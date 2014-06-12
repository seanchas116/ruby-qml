#pragma once

#include <ruby.h>
#include <ruby/encoding.h>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include "util.h"

namespace RubyQml {

template <typename T> T fromRuby(VALUE x);
template <typename T> VALUE toRuby(const T &value);

namespace detail {

template <typename T, typename Enable = void> struct Conversion;

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_signed<T>::value && std::is_integral<T>::value>::type>
{
    static T from(VALUE x)
    {
        T ret;
        protect([&] {
            ret = NUM2LL(x);
        });
        return ret;
    }
    static VALUE to(T x)
    {
        return LL2NUM(x);
    }
};

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_unsigned<T>::value && std::is_integral<T>::value>::type>
{
    static T from(VALUE x)
    {
        T ret;
        protect([&] {
            ret = NUM2ULL(x);
        });
        return ret;
    }
    static VALUE to(T x)
    {
        return ULL2NUM(x);
    }
};

template <typename T>
struct Conversion<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    static T from(VALUE x)
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
    static VALUE to(T x)
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
    static T<V> from(VALUE x)
    {
        protect([&] {
            x = rb_convert_type(x, T_ARRAY, "Array", "to_ary");
        });
        int length = RARRAY_LEN(x);
        T<V> list;
        list.reserve(length);
        for (int i = 0; i < length; ++i) {
            list << fromRuby<V>(RARRAY_AREF(x, i));
        }
        return list;
    }
    static VALUE to(const T<V> &list)
    {
        VALUE ary;
        protect([&] {
            ary = rb_ary_new();
        });
        for (const auto &elem : list) {
            auto x = toRuby(elem);
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
    static T<K, V> from(VALUE x)
    {
        protect([&] {
            x = rb_convert_type(x, T_HASH, "Hash", "to_hash");
        });
        T<K, V> hash;
        protect([&] {
            auto each = [](VALUE key, VALUE value, VALUE arg) -> int {
                auto &hash = *reinterpret_cast<T<K, V> *>(arg);
                unprotect([&] {
                    hash[fromRuby<K>(key)] = fromRuby<V>(value);
                });
                return ST_CONTINUE;
            };
            auto eachPtr = (int (*)(VALUE, VALUE, VALUE))each;
            rb_hash_foreach(x, (int (*)(...))eachPtr, (VALUE)(&hash));
        });
        return hash;
    }
    static VALUE to(const T<K, V> &hash)
    {
        VALUE rubyHash;
        protect([&] {
            rubyHash = rb_hash_new();
        });
        for (auto i = hash.begin(); i != hash.end(); ++i) {
            auto k = toRuby(i.key());
            auto v = toRuby(i.value());
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
    static bool from(VALUE value) { return RTEST(value); }
    static VALUE to(bool x) { return x ? Qtrue : Qfalse; }
};

template <>
struct Conversion<const char *>
{
    static VALUE to(const char *str);
};

template <>
struct Conversion<QByteArray>
{
    static QByteArray from(VALUE x);
    static VALUE to(const QByteArray &str);
};

template <>
struct Conversion<QString>
{
    static QString from(VALUE x);
    static VALUE to(const QString &str);
};

template <>
struct Conversion<QDateTime>
{
    static QDateTime from(VALUE x);
    static VALUE to(const QDateTime &dateTime);
};

template <>
struct Conversion<QObject *>
{
    static QObject *from(VALUE x);
    static VALUE to(QObject *obj);
};

template <>
struct Conversion<QVariant>
{
    static QVariant from(VALUE x);
    static VALUE to(const QVariant &variant);
};

template <>
struct Conversion<const QMetaObject *>
{
    static const QMetaObject *from(VALUE x);
    static VALUE to(const QMetaObject *metaobj);
};

} // namespace detail

template <typename T>
inline T fromRuby(VALUE x)
{
    return detail::Conversion<T>::from(x);
}

template <typename T>
inline VALUE toRuby(const T &value)
{
    return detail::Conversion<T>::to(value);
}

bool convertibleTo(VALUE value, int metaType);
int defaultMetaTypeFor(VALUE value);

QVariant fromRuby(VALUE x, int type);


ID idFromValue(VALUE sym);

VALUE echoConversion(VALUE value);

} // namespace RubyQml
