#pragma once
#include "util.h"
#include <ruby.h>

class QVariant;

namespace RubyQml {

namespace detail {
template <typename T, typename Enable = void> struct Conversion;
}

class RubyValue
{
public:
    RubyValue() = default;
    RubyValue(VALUE value) : mValue(value) {}

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

    bool operator==(const RubyValue &other) const { return mValue == other.mValue; }
    bool operator!=(const RubyValue &other) const { return !operator==(other); }
    explicit operator bool() const { return RTEST(mValue); }

    bool isKindOf(RubyValue klass) const;
    bool isConvertibleTo(int metaType) const;
    int defaultMetaType() const;

    static RubyValue from(const QVariant &value);
    QVariant toVariant() const;
    QVariant toVariant(int type) const;

    ID toID() const;
    static RubyValue fromID(ID id) { return ID2SYM(id); }

private:
    volatile VALUE mValue = Qnil;
};

namespace detail {

template <>
struct Conversion<RubyValue>
{
    static RubyValue from(RubyValue x) { return x; }
    static RubyValue to(RubyValue x) { return x;}
};

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

template <>
struct Conversion<bool>
{
    static bool from(RubyValue value) { return value; }
    static RubyValue to(bool x) { return x ? Qtrue : Qfalse; }
};

template <>
struct Conversion<const char *>
{
    static RubyValue to(const char *str);
};

template <>
struct Conversion<QByteArray>
{
    static QByteArray from(RubyValue x);
    static RubyValue to(const QByteArray &str);
};

template <>
struct Conversion<QString>
{
    static QString from(RubyValue x);
    static RubyValue to(const QString &str);
};

template <>
struct Conversion<QDateTime>
{
    static QDateTime from(RubyValue x);
    static RubyValue to(const QDateTime &dateTime);
};

template <>
struct Conversion<QObject *>
{
    static QObject *from(RubyValue x);
    static RubyValue to(QObject *obj);
};

template <>
struct Conversion<QVariant>
{
    static QVariant from(RubyValue x);
    static RubyValue to(const QVariant &variant);
};

template <>
struct Conversion<const QMetaObject *>
{
    static const QMetaObject *from(RubyValue x);
    static RubyValue to(const QMetaObject *metaobj);
};

} // namespace detail

} // namespace RubyQml
