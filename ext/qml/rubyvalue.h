#pragma once
#include "util.h"
#include "functioninfo.h"
#include <ruby.h>

class QVariant;

namespace RubyQml {

namespace detail {
template <typename T, typename Enable = void> struct Conversion
{
    static T from(RubyValue x);
    static RubyValue to(typename std::conditional<std::is_scalar<T>::value, T, const T &>::type str);
};

}

class RubyValue
{
public:
    RubyValue() = default;
    RubyValue(VALUE value) : mValue(value) {}

    static RubyValue fromPath(const char *path);

    template <typename T> static RubyValue from(const T &value)
    {
        return detail::Conversion<T>::to(value);
    }

    template <typename T> T to() const
    {
        return detail::Conversion<T>::from(*this);
    }

    operator VALUE() const { return mValue; }

    template <typename ... TArgs>
    RubyValue send(const char *method, TArgs && ... args) const
    {
        RubyValue ret;
        protect([&] {
            ret = rb_funcall(mValue, rb_intern(method), sizeof...(args), VALUE(args)...);
        });
        return ret;
    }

    template <typename TFunction, TFunction function>
    void defineSingletonMethod(const char *name, FunctionInfo<TFunction, function>)
    {
        using wrapper = FunctionWrapper<TFunction, function>;
        auto func = (VALUE (*)(...))wrapper::apply;
        auto argc = wrapper::argc - 1;

        protect([&] {
            rb_define_singleton_method(mValue, name, func, argc);
        });
    }

    bool operator==(const RubyValue &other) const { return mValue == other.mValue; }
    bool operator!=(const RubyValue &other) const { return !operator==(other); }
    explicit operator bool() const { return RTEST(mValue); }

    bool isKindOf(RubyValue klass) const;
    bool isConvertibleTo(int metaType) const;
    int defaultMetaType() const;

    static RubyValue fromVariant(const QVariant &value);
    QVariant toVariant() const;
    QVariant toVariant(int type) const;
    static RubyValue fromQObject(QObject *obj, bool implicit = true);
    QObject *toQObject() const;

    ID toID() const;
    static RubyValue fromID(ID id) { return ID2SYM(id); }

    static void addEnumeratorMetaType(int metaType);

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

    volatile VALUE mValue = Qnil;
};

namespace detail {

template <>
struct Conversion<RubyValue>
{
    static RubyValue from(RubyValue x) { return x; }
    static RubyValue to(RubyValue x) { return x; }
};

template <>
struct Conversion<bool>
{
    static bool from(RubyValue value) { return value; }
    static RubyValue to(bool x) { return x ? Qtrue : Qfalse; }
};

// signed integers

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_signed<T>::value && std::is_integral<T>::value>::type>
{
    static T from(RubyValue x)
    {
        T ret;
        protect([&] {
            ret = NUM2LL(x);
        });
        return ret;
    }
    static RubyValue to(T x)
    {
        return LL2NUM(x);
    }
};

// unsigned integers

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_unsigned<T>::value && std::is_integral<T>::value>::type>
{
    static T from(RubyValue x)
    {
        T ret;
        protect([&] {
            ret = NUM2ULL(x);
        });
        return ret;
    }
    static RubyValue to(T x)
    {
        return ULL2NUM(x);
    }
};

// floating point values

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    static T from(RubyValue x)
    {
        auto type = rb_type(x);
        if (type == T_FIXNUM || type == T_BIGNUM) {
            return NUM2LL(x);
        }

        T ret;
        protect([&] {
            ret = rb_float_value(x);
        });
        return ret;
    }
    static RubyValue to(T x)
    {
        return rb_float_new(x);
    }
};

// QList and QVector

template <class T> struct IsQListLike : std::false_type {};
template <class V> struct IsQListLike<QList<V>> : std::true_type {};
template <class V> struct IsQListLike<QVector<V>> : std::true_type {};

template <template<class> class T, class V>
struct Conversion<T<V>, typename std::enable_if<IsQListLike<T<V>>::value>::type>
{
    static T<V> from(RubyValue x)
    {
        protect([&] {
            x = rb_convert_type(x, T_ARRAY, "Array", "to_ary");
        });
        int length = RARRAY_LEN(VALUE(x));
        T<V> list;
        list.reserve(length);
        for (int i = 0; i < length; ++i) {
            list << RubyValue(RARRAY_AREF(VALUE(x), i)).to<V>();
        }
        return list;
    }
    static RubyValue to(const T<V> &list)
    {
        RubyValue ary;
        protect([&] {
            ary = rb_ary_new();
        });
        for (const auto &elem : list) {
            auto x = RubyValue::from(elem);
            protect([&] {
                rb_ary_push(ary, x);
            });
        }
        return ary;
    }
};

// QHash and QMap

template <class T> struct IsQHashLike : std::false_type {};
template <class K, class V> struct IsQHashLike<QHash<K, V>> : std::true_type {};
template <class K, class V> struct IsQHashLike<QMap<K, V>> : std::true_type {};

template <template<class, class> class T, class K, class V>
struct Conversion<T<K, V>, typename std::enable_if<IsQHashLike<T<K, V>>::value>::type>
{
    static T<K, V> from(RubyValue x)
    {
        protect([&] {
            x = rb_convert_type(x, T_HASH, "Hash", "to_hash");
        });
        T<K, V> hash;
        protect([&] {
            auto each = [](VALUE key, VALUE value, VALUE arg) -> int {
                auto &hash = *reinterpret_cast<T<K, V> *>(arg);
                unprotect([&] {
                    hash[RubyValue(key).to<K>()] = RubyValue(value).to<V>();
                });
                return ST_CONTINUE;
            };
            auto eachPtr = (int (*)(VALUE, VALUE, VALUE))each;
            rb_hash_foreach(x, (int (*)(...))eachPtr, (VALUE)(&hash));
        });
        return hash;
    }
    static RubyValue to(const T<K, V> &hash)
    {
        RubyValue rubyHash;
        protect([&] {
            rubyHash = rb_hash_new();
        });
        for (auto i = hash.begin(); i != hash.end(); ++i) {
            auto k = RubyValue::from(i.key());
            auto v = RubyValue::from(i.value());
            protect([&] {
                rb_hash_aset(rubyHash, k, v);
            });
        }
        return rubyHash;
    }
};

// QObject-derived

template <typename T>
struct Conversion<
    T *,
    typename std::enable_if<
        std::is_base_of<QObject, T>::value
    >::type
>
{
    static T *from(RubyValue value) { return dynamic_cast<T *>(value.toQObject()); }
    static RubyValue to(T *value) { return RubyValue::fromQObject(value); }
};

template <>
struct Conversion<const char *>
{
    static RubyValue to(const char *str);
};

} // namespace detail

} // namespace RubyQml
