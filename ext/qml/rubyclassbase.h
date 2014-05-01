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
                rb_raise(rb_eTypeError, "the value is not a %s", rb_class2name(klass));
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

    class Definition;

    static VALUE rubyClass()
    {
        return definition().rubyClass();
    }

    static Definition definition()
    {
        if (mDefinition.rubyClass() == Qnil) {
            mDefinition = TDerived::createDefinition();
        }
        return mDefinition;
    }

    static void defineClass()
    {
        definition();
    }

private:

    static void markImpl(TDerived *ptr)
    {
        ptr->mark();
    }

    static void dealloc(TDerived *ptr)
    {
        ptr->~TDerived();
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

    static Definition mDefinition;
};

template <typename TDerived>
typename RubyClassBase<TDerived>::Definition RubyClassBase<TDerived>::mDefinition;

namespace detail {

template <typename T>
struct Conversion<T *, typename std::enable_if<std::is_base_of<RubyClassBase<T>, T>::value>::type>
{
    static T *from(VALUE value)
    {
        return T::getPointer(value);
    }
};

}

template <typename TDerived>
class RubyClassBase<TDerived>::Definition
{
public:

    Definition() {}

    Definition(const char *outerPath, const char *name)
    {
        protect([&] {
            mKlass = rb_define_class_under(rb_path2class(outerPath), name, rb_cObject);
            rb_define_alloc_func(mKlass, &alloc);
        });
    }

    VALUE rubyClass() { return mKlass; }

    template <typename TMemberFunction, TMemberFunction memfn>
    Definition &defineMethod(const char *name, MethodAccess access = MethodAccess::Public)
    {
        switch (access) {
        case MethodAccess::Public:
            MethodDefinition<TMemberFunction, memfn>::apply(&rb_define_method, mKlass, name);
            break;
        case MethodAccess::Protected:
            MethodDefinition<TMemberFunction, memfn>::apply(&rb_define_protected_method, mKlass, name);
            break;
        case MethodAccess::Private:
            MethodDefinition<TMemberFunction, memfn>::apply(&rb_define_private_method, mKlass, name);
            break;
        }
        return *this;
    }

    Definition &aliasMethod(const char *newName, const char *oldName)
    {
        protect([&] {
            rb_alias(mKlass, rb_intern(newName), rb_intern(oldName));
        });
        return *this;
    }

private:

    using DefineFunc = void (*)(VALUE, const char *, VALUE (*)(...), int);

    template <typename TMemberFunction, TMemberFunction memfn, typename ... TArgs>
    struct MethodImpl
    {
        static VALUE apply(VALUE self, TArgs ... args)
        {
            return unprotect([&] {
                return (fromRuby<TDerived *>(self)->*memfn)(args ...);
            });
        }
    };

    template <typename TMemberFunction, TMemberFunction memfn>
    struct MethodDefinition;

    template <typename ... TArgs, VALUE (TDerived::*memfn)(TArgs ...)>
    struct MethodDefinition<VALUE (TDerived::*)(TArgs ...), memfn>
    {
        using Impl = MethodImpl<VALUE (TDerived::*)(TArgs ...), memfn, TArgs ...>;

        static void apply(DefineFunc define, VALUE klass, const char *name)
        {
            protect([&] {
                define(klass, name, (VALUE (*)(...))(&Impl::apply), sizeof...(TArgs));
            });
        }
    };

    template <typename ... TArgs, VALUE (TDerived::*memfn)(TArgs ...) const>
    struct MethodDefinition<VALUE (TDerived::*)(TArgs ...) const, memfn>
    {
        using Impl = MethodImpl<VALUE (TDerived::*)(TArgs ...) const, memfn, TArgs ...>;

        static void apply(DefineFunc define, VALUE klass, const char *name)
        {
            protect([&] {
                define(klass, name, (VALUE (*)(...))(&Impl::apply), sizeof...(TArgs));
            });
        }
    };


    VALUE mKlass = Qnil;
};

}
