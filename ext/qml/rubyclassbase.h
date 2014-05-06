#pragma once
#include "util.h"
#include "conversion.h"

#define METHOD_TYPE_NAME(name) decltype(name), name

namespace RubyQml {

enum class MethodAccess
{
    Public,
    Protected,
    Private
};

template <typename TDerived>
class RubyClassBase
{
public:

    VALUE self() { return mSelf; }

    static TDerived *getPointer(VALUE value)
    {
        auto klass = rubyClass();
        protect([&] {
            if (!RTEST(rb_obj_is_kind_of(value, klass))) {
                rb_raise(rb_eTypeError, "expected %s, got %s", rb_class2name(klass), rb_obj_classname(value));
            }
        });
        TDerived *ptr;
        Data_Get_Struct(value, TDerived, ptr);
        return ptr;
    }

    static VALUE newAsRuby()
    {
        auto klass = rubyClass();
        return protect([&] {
            return rb_obj_alloc(klass);
        });
    }

    class ClassBuilder;

    static VALUE rubyClass()
    {
        if (mKlass == Qnil) {
            mKlass = TDerived::buildClass().rubyClass();
        }
        return mKlass;
    }

    static void defineClass()
    {
        rubyClass();
    }

private:

    static void markImpl(TDerived *ptr)
    {
        ptr->mark();
    }

    static void dealloc(TDerived *ptr)
    {
        withoutGvl([&] {
            ptr->~TDerived();
        });
        ruby_xfree(ptr);
    }

    static VALUE alloc(VALUE klass)
    {
        auto ptr = ruby_xmalloc(sizeof(TDerived));
        new(ptr) TDerived();
        auto self = Data_Wrap_Struct(klass, &markImpl, &dealloc, ptr);
        static_cast<TDerived *>(ptr)->mSelf = self;
        return self;
    }

    VALUE mSelf;

    static VALUE mKlass;
};

template <typename TDerived>
VALUE RubyClassBase<TDerived>::mKlass = Qnil;

template <typename TDerived>
class RubyClassBase<TDerived>::ClassBuilder
{
public:

    ClassBuilder(const char *outerPath, const char *name)
    {
        protect([&] {
            mKlass = rb_define_class_under(rb_path2class(outerPath), name, rb_cObject);
            rb_define_alloc_func(mKlass, &alloc);
        });
    }

    VALUE rubyClass() { return mKlass; }

    template <typename TMemberFunction, TMemberFunction memfn>
    ClassBuilder &defineMethod(const char *name, MethodAccess access = MethodAccess::Public)
    {
        using wrapper = MethodWrapper<TMemberFunction, memfn>;
        auto func = (VALUE (*)(...))&wrapper::apply;
        auto argc = wrapper::argc;

        protect([&] {
            switch (access) {
            case MethodAccess::Public:
                rb_define_method(mKlass, name, func, argc);
                break;
            case MethodAccess::Protected:
                rb_define_protected_method(mKlass, name, func, argc);
                break;
            case MethodAccess::Private:
                rb_define_private_method(mKlass, name, func, argc);
                break;
            }
        });
        return *this;
    }

    ClassBuilder &aliasMethod(const char *newName, const char *oldName)
    {
        protect([&] {
            rb_alias(mKlass, rb_intern(newName), rb_intern(oldName));
        });
        return *this;
    }

private:

    template <typename TMemberFunction, TMemberFunction memfn>
    struct MethodWrapper;

    template <typename ... TArgs, VALUE (TDerived::*memfn)(TArgs ...)>
    struct MethodWrapper<VALUE (TDerived::*)(TArgs ...), memfn>
    {
        static constexpr size_t argc = sizeof...(TArgs);

        static VALUE apply(VALUE self, TArgs ... args)
        {
            return unprotect([&] {
                return (getPointer(self)->*memfn)(args ...);
            });
        }
    };

    template <typename ... TArgs, VALUE (TDerived::*memfn)(TArgs ...) const>
    struct MethodWrapper<VALUE (TDerived::*)(TArgs ...) const, memfn>
    {
        static constexpr size_t argc = sizeof...(TArgs);

        static VALUE apply(VALUE self, TArgs ... args)
        {
            return unprotect([&] {
                return (getPointer(self)->*memfn)(args ...);
            });
        }
    };

    VALUE mKlass = Qnil;
};

}
