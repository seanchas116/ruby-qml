#pragma once

#include <ruby.h>
#include <ruby/encoding.h>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include "util.h"

namespace RubyQml {

ID idFromValue(VALUE sym);

inline VALUE toRuby(bool b) { return b ? Qtrue : Qfalse; }

inline VALUE toRuby(char n) { return LL2NUM(n); }
inline VALUE toRuby(short n) { return LL2NUM(n); }
inline VALUE toRuby(long n) { return LL2NUM(n); }
inline VALUE toRuby(long long n) { return LL2NUM(n); }

inline VALUE toRuby(unsigned char n) { return ULL2NUM(n); }
inline VALUE toRuby(unsigned short n) { return ULL2NUM(n); }
inline VALUE toRuby(unsigned long n) { return ULL2NUM(n); }
inline VALUE toRuby(unsigned long long n) { return ULL2NUM(n); }

inline VALUE toRuby(int n) { return LL2NUM(n); }
inline VALUE toRuby(unsigned int n) { return ULL2NUM(n); }

inline VALUE toRuby(double x) { return rb_float_new(x); }
inline VALUE toRuby(float x) { return rb_float_new(x); }

VALUE toRuby(const char *str);
VALUE toRuby(const QByteArray &str);
VALUE toRuby(const QString &str);

VALUE toRuby(const QDateTime &dateTime);
VALUE toRuby(const QVariant &variant);

template <class T>
VALUE qListLikeToRuby(const T &list)
{
    return protect([&] {
        auto ary = rb_ary_new();
        for (const auto &elem : list) {
            rb_ary_push(ary, toRuby(elem));
        }
        return ary;
    });
}

template <class T>
VALUE qHashLikeToRuby(const T &hash)
{
    return protect([&] {
        auto rubyHash = rb_hash_new();
        for (auto i = hash.begin(); i != hash.end(); ++i) {
            rb_hash_aset(rubyHash, toRuby(i.key()), toRuby(i.value()));
        }
        return rubyHash;
    });
}

template <typename V>
inline VALUE toRuby(const QList<V> &list) { return qListLikeToRuby(list); }
template <typename V>
inline VALUE toRuby(const QVector<V> &list) { return qListLikeToRuby(list); }
template <typename K, typename V>
inline VALUE toRuby(const QHash<K, V> &hash) { return qHashLikeToRuby(hash); }
template <typename K, typename V>
inline VALUE toRuby(const QMap<K, V> &hash) { return qHashLikeToRuby(hash); }


template <typename T, typename Enable = void>
struct FromRuby;

template <typename T>
inline T fromRuby(VALUE x)
{
    return FromRuby<T>::apply(x);
}

template <>
inline bool fromRuby<bool>(VALUE x) { return RTEST(x); }

template <typename T>
struct FromRuby<T, typename std::enable_if<std::is_signed<T>::value && std::is_integral<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protect([&] {
            return NUM2LL(x);
        });
    }
};

template <typename T>
struct FromRuby<T, typename std::enable_if<std::is_unsigned<T>::value && std::is_integral<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protect([&] {
            return NUM2ULL(x);
        });
    }
};

template <typename T>
struct FromRuby<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protect([&] {
            return rb_float_value(x);
        });
    }
};

template <>
QString fromRuby<QString>(VALUE x)
{
    protect([&] {
        StringValue(x);
    });
    return QString::fromUtf8(RSTRING_PTR(x), RSTRING_LEN(x));
}

template <>
QByteArray fromRuby<QByteArray>(VALUE x)
{
    protect([&] {
        StringValue(x);
    });
    return QByteArray(RSTRING_PTR(x), RSTRING_LEN(x));
}

template <typename T>
T qListLikeFromRuby(VALUE x)
{
    protect([&] {
        x = rb_check_array_type(x);
    });
    int length = RARRAY_LEN(x);
    T list;
    list.reserve(length);
    for (int i = 0; i < length; ++i) {
        list << fromRuby<typename T::value_type>(RARRAY_AREF(x, i));
    }
    return list;
}

template <typename T>
T qHashLikeFromRuby(VALUE x)
{
    using Key = typename T::key_type;
    using Value = typename decltype(std::declval<T>().values())::value_type;
    protect([&] {
        x = rb_check_hash_type(x);
    });
    T hash;
    protect([&] {
        auto each = [](VALUE key, VALUE value, VALUE arg) -> int {
            auto &hash = *reinterpret_cast<T *>(arg);
            hash[fromRuby<Key>(key)] = fromRuby<Value>(value);
            return ST_CONTINUE;
        };
        auto eachPtr = (int (*)(VALUE, VALUE, VALUE))each;
        rb_hash_foreach(x, (int (*)(...))eachPtr, (VALUE)(&hash));
    });
    return hash;
}

template <class V>
struct FromRuby<QList<V>>
{
    static QList<V> apply(VALUE x) { return qListLikeFromRuby<QList<V>>(x); }
};
template <class V>
struct FromRuby<QVector<V>>
{
    static QVector<V> apply(VALUE x) { return qListLikeFromRuby<QVector<V>>(x); }
};
template <class K, class V>
struct FromRuby<QHash<K, V>>
{
    static QHash<K, V> apply(VALUE x) { return qHashLikeFromRuby<QHash<K, V>>(x); }
};
template <class K, class V>
struct FromRuby<QMap<K, V>>
{
    static QMap<K, V> apply(VALUE x) { return qHashLikeFromRuby<QMap<K, V>>(x); }
};

template<>
QDateTime fromRuby<QDateTime>(VALUE x)
{
    return protect([&] {
        auto at = rb_funcall(x, rb_intern("to_r"), 0);
        int num = RRATIONAL(at)->num;
        int den = RRATIONAL(at)->den;
        return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
    });
}

template <>
QObject *fromRuby<QObject *>(VALUE x);

enum class TypeCategory
{
    Invalid,
    Integer,
    Float,
    String,
    Array,
    Hash,
    Time,
    QtObject
};

TypeCategory metaTypeToCategory(int metaType);
int categoryToMetaType(TypeCategory category);
TypeCategory rubyValueCategory(VALUE x);

QVariant fromRuby(VALUE x, int type);

template <>
inline QVariant fromRuby<QVariant>(VALUE x)
{
    return fromRuby(x, -1);
}

} // namespace RubyQml
