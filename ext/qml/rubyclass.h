#pragma once
#include "functioninfo.h"
#include "rubyvalue.h"
#include <QDebug>

namespace RubyQml {

enum class MethodAccess { Public, Protected, Private };
enum class MethodType { InstanceMethod, ModuleFunction };

class RubyModule
{
public:
    RubyModule() = default;
    RubyModule(VALUE moduleValue);
    RubyModule(RubyValue moduleValue);
    RubyModule(const char *name);
    RubyModule(const RubyModule &under, const char *name);
    RubyModule(const RubyModule &other) = default;
    RubyModule &operator=(const RubyModule &other);

    static RubyModule fromPath(const char *path);

    template <typename TFunction, TFunction function>
    void defineMethod(MethodType type, MethodAccess access, const char *name, FunctionInfo<TFunction, function>)
    {
        using wrapper = FunctionWrapper<TFunction, function>;
        auto func = (VALUE (*)(...))wrapper::apply;
        auto argc = wrapper::argc - 1;

        protect([&] {
            switch (type) {
            case MethodType::InstanceMethod:
                switch (access) {
                case MethodAccess::Public:
                    rb_define_method(mValue , name, func, argc);
                    break;
                case MethodAccess::Protected:
                    rb_define_protected_method(mValue, name, func, argc);
                    break;
                case MethodAccess::Private:
                    rb_define_private_method(mValue, name, func, argc);
                    break;
                }
                break;
            case MethodType::ModuleFunction:
                rb_define_module_function(mValue, name, func, argc);
                break;
            }
        });
    }

    template <typename T>
    void defineMethod(MethodAccess access, const char *name, T info)
    {
        defineMethod(MethodType::InstanceMethod, access, name, info);
    }

    template <typename T>
    void defineModuleFunction(const char *name, T info)
    {
        defineMethod(MethodType::ModuleFunction, MethodAccess::Public, name, info);
    }

    template <typename T>
    void defineMethod(const char *name, T info)
    {
        defineMethod(MethodAccess::Public, name, info);
    }

    void aliasMethod(const char *newName, const char *originalName);

    RubyValue toValue() const { return mValue; }
    operator RubyValue() const { return toValue(); }
    operator VALUE() const { return toValue(); }

    virtual void checkType();

private:
    template <typename TFunction, TFunction function>
    struct FunctionWrapper;

    template <typename ... TArgs, RubyValue (*function)(TArgs...)>
    struct FunctionWrapper<RubyValue (*)(TArgs...), function>
    {
        static constexpr size_t argc = sizeof...(TArgs);

        static VALUE apply(typename std::conditional<true, VALUE, TArgs>::type... args)
        {
            RubyValue ret;
            unprotect([&] {
                ret = function(RubyValue(args)...);
            });
            return ret;
        }
    };

    RubyValue mValue;
};

class RubyClass : public RubyModule
{
public:
    RubyClass() = default;
    RubyClass(RubyValue classValue);
    RubyClass(const RubyModule &under, const char *name);

    static RubyClass fromPath(const char *path);

    void checkType() override;

    template <typename ... Args>
    RubyValue newInstance(Args ... args)
    {
        return toValue().send("new", args...);
    }
};


template <typename T>
class WrapperRubyClass : public RubyClass
{
public:
    WrapperRubyClass(const RubyModule &under, const char *name) :
        RubyClass(under, name)
    {
        if (mInstance) {
            throw std::logic_error("class already defined");
        }
        mInstance.reset(new WrapperRubyClass(*this));

        protect([&] {
            rb_define_alloc_func(toValue(), &alloc);
        });
    }

    T *unwrap(RubyValue value)
    {
        auto klass = this->toValue();
        protect([&] {
            if (!RTEST(rb_obj_is_kind_of(value, klass))) {
                rb_raise(rb_eTypeError, "expected %s, got %s", rb_class2name(klass), rb_obj_classname(value));
            }
        });
        T *ptr;
        Data_Get_Struct(VALUE(value), T, ptr);
        return ptr;
    }

    using RubyClass::defineMethod;

    template <typename TMemberFunction, TMemberFunction memberFunction>
    void defineMethod(MethodAccess access, const char *name, MemberFunctionInfo<TMemberFunction, memberFunction>)
    {
        using wrapper = MethodWrapper<TMemberFunction, memberFunction>;
        defineMethod(access, name, RUBYQML_FUNCTION_INFO(&wrapper::apply));
    }

    template <typename TMemberFunction, TMemberFunction memberFunction>
    void defineMethod(const char *name, MemberFunctionInfo<TMemberFunction, memberFunction> info)
    {
        defineMethod(MethodAccess::Public, name, info);
    }

    static WrapperRubyClass instance()
    {
        if (!mInstance) {
            throw std::logic_error("class not yet defined");
        }
        return *mInstance;
    }

private:

    template <typename TMemberFunction, TMemberFunction memfn>
    struct MethodWrapper;

    template <typename ... TArgs, RubyValue (T::*memfn)(TArgs ...)>
    struct MethodWrapper<RubyValue (T::*)(TArgs ...), memfn>
    {
        static RubyValue apply(RubyValue self, TArgs ... args)
        {
            return (instance().unwrap(self)->*memfn)(args ...);
        }
    };

    template <typename ... TArgs, RubyValue (T::*memfn)(TArgs ...) const>
    struct MethodWrapper<RubyValue (T::*)(TArgs ...) const, memfn>
    {
        static RubyValue apply(RubyValue self, TArgs... args)
        {
            return (instance().unwrap(self)->*memfn)(args ...);
        }
    };

    static void mark(T *ptr) noexcept
    {
        ptr->gc_mark();
    }

    static void dealloc(T *ptr) noexcept
    {
        withoutGvl([&] {
            ptr->~T();
        });
        ruby_xfree(ptr);
    }

    static VALUE alloc(VALUE klass) noexcept
    {
        auto ptr = ruby_xmalloc(sizeof(T));
        auto self = Data_Wrap_Struct(klass, &mark, &dealloc, ptr);
        new(ptr) T(self);
        return self;
    }

    static std::unique_ptr<WrapperRubyClass> mInstance;
};

template <typename T>
std::unique_ptr<WrapperRubyClass<T>> WrapperRubyClass<T>::mInstance = nullptr;

template <typename T>
inline WrapperRubyClass<T> wrapperRubyClass()
{
    return WrapperRubyClass<T>::instance();
}

} // namespace RubyQml

