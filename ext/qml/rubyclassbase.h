#pragma once
#include "util.h"

namespace RubyQml {

#define METHOD_TYPE_POINTER(ptr) decltype(ptr), ptr

template <typename TDerived>
class RubyClassBase
{
public:

    template <typename ... Args>
    static VALUE newAsRuby(Args && ... args);

    static TDerived *fromRuby(VALUE value)
    {
        protectedCall([&] {
            if (rb_obj_is_kind_of(value, rubyClass()) == Qfalse) {
                rb_raise(rb_eTypeError, "the value is not a %s", rb_class2name(rubyClass()));
            }
        });
        TDerived *ptr;
        Data_Get_Struct(value, TDerived, ptr);
        return ptr;
    }

    static VALUE rubyClass()
    {
        return definition().rubyClass();
    }

    static Definition &definition()
    {
        if (!mDefinition) {
            mDefinition.reset(TDerived::defineClass());
        }
        return *mDefinition;
    }

    class Definition
    {
    public:

        Definition(const char *outerPath, const char *name)
        {
            protectedCall([&] {
                rb_define_class_under(rb_path2class(outerPath), name, rb_cObject);
                rb_define_private_method();
            });
        }

        VALUE rubyClass() { return mKlass; }

        template <typename TMemberFunction, TMemberFunction memfn>
        Definition &defineMethod(const char *name)
        {
            MethodDefinition<&rb_define_method, TMemberFunction, memfn>::apply(mKlass, name);
            return *this;
        }
        template <typename TMemberFunction, TMemberFunction memfn>
        Definition &definePrivateMethod(const char *name)
        {
            MethodDefinition<&rb_define_private_method, TMemberFunction, memfn>::apply(mKlass, name);
            return *this;
        }
        template <typename TMemberFunction, TMemberFunction memfn>
        Definition &defineProtectedMethod(const char *name)
        {
            MethodDefinition<&rb_define_protected_method, TMemberFunction, memfn>::apply(mKlass, name);
            return *this;
        }

    private:

        using DefineFunc = void (*)(VALUE, const char *, VALUE (*func)(...), int);

        template <typename TFunc, TFunc func>
        struct ExceptionReRaise;

        template <typename ... TArgs, TFunc func>
        struct ExceptionReRaise<VALUE (*)(TArgs...), func>
        {
            static VALUE apply(TArgs ... args)
            {
                int excState = 0;
                try {
                    return func(args...);
                } catch (const RubyException &exc) {
                    excState = exc.state();
                }
                rb_jump_tag(excState);
                return Qnil;
            }
        };

        template <DefineFunc define, typename TMemberFunction, TMemberFunction memfn>
        struct MethodDefinition;

        template <DefineFunc define, typename ... TArgs, VALUE (TDerived::*memfn)(TArgs ...)>
        struct MethodDefinition<define, decltype(memfn), memfn>
        {
            static VALUE callMethod(VALUE self, TArgs ... args)
            {
                return getStruct<TDerived>(self)->*memfn(args ...);
            }

            using ReRaise = ExceptionReRaise<decltype(&callmethod), &callMethod>;

            static void apply(VALUE klass, const char *name)
            {
                protectedCall([&] {
                    define(klass, name, static_cast<VALUE (*)(...)>(&ReRaise::apply), sizeof...(TArgs));
                });
            }
        };

        template <DefineFunc define, VALUE (TDerived::*memfn)(int, VALUE *)>
        struct MethodDefinition<define, decltype(memfn), memfn>
        {
            static VALUE callMethod(int argc, VALUE *argv, VALUE self)
            {
                return getStruct<TDerived>(self)->*memfn(argc, argv);
            }

            using ReRaise = ExceptionReRaise<decltype(&callMethod), &callMethod>;

            static void apply(VALUE klass, const char *name)
            {
                protectedCall([&] {
                    define(klass, name, static_cast<VALUE (*)(...))>(&ReRaise::apply), -1);
                });
            }
        };

        VALUE mKlass;
    };

private:

    static void markImpl(TDerived *ptr)
    {
        ptr->mark();
    }

    static void *dealloc(TDerived *ptr)
    {
        ptr->~TDerived();
        ruby_xfree(ptr);
    }

    static VALUE alloc(VALUE klass)
    {
        auto ptr = ruby_xmalloc(sizeof(TDerived));
        new(ptr) TDerived();
        return Data_Wrap_Struct(klass, &markImpl, &dealloc, ptr);
    }

    static std::unique_ptr<Definition> mDefinition;
};

template <typename TDerived>
std::unique_ptr<RubyClassBase<TDerived>::Definition> RubyClassBase<TDerived>::mDefinition;

}
