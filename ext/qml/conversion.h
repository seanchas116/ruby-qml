#pragma once

#include <ruby.h>
#include <ruby/encoding.h>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include "util.h"

namespace RubyQml {


inline VALUE toRuby(VALUE x) { return x; }
inline VALUE toRuby(ID x) { return ID2SYM(x); }

inline VALUE toRuby(bool b) { return b ? Qtrue : Qfalse; }
inline VALUE toRuby(qlonglong n) { return LL2NUM(n); }
inline VALUE toRuby(qulonglong n) { return ULL2NUM(n); }
inline VALUE toRuby(double x) { return rb_float_new(x); }

inline VALUE toRuby(const QByteArray &str);
inline VALUE toRuby(const QString &str) { return toRuby(str.toUtf8()); }

template <class T>
VALUE qListLikeToRuby(const T &list)
{
    return protectedCall([&] {
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
    return protectedCall([&] {
        auto rubyHash = rb_hash_new();
        for (auto i = hash.begin(); i != hash.end; ++i) {
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

VALUE toRuby(const QDateTime &dateTime);
inline VALUE toRuby(const QDate &date) { return toRuby(QDateTime(date)); }

VALUE toRuby(const QVariant &variant);

template <typename T, typename Enable = void>
struct FromRuby;

template <typename T>
inline T fromRuby(VALUE x)
{
    return FromRuby<T>::apply(x);
}

template <>
ID fromRuby<ID>(VALUE x)
{
    return protectedCall([&] {
        x = rb_check_convert_type(x, T_SYMBOL, "Symbol", "to_sym");
        return SYM2ID(x);
    });
}

template <>
inline bool fromRuby<bool>(VALUE x) { return RTEST(x); }

template <typename T>
struct FromRuby<T, std::enable_if<std::is_signed<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protectedCall([&] {
            return NUM2LL(x);
        });
    }
};

template <typename T>
struct FromRuby<T, std::enable_if<std::is_unsigned<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protectedCall([&] {
            return NUM2ULL(x);
        });
    }
};

template <typename T>
struct FromRuby<T, std::enable_if<std::is_floating_point<T>::value>::type>
{
    static T apply(VALUE x)
    {
        return protectedCall([&] {
            return rb_float_value(x);
        });
    }
};

template <>
QString fromRuby<QString>(VALUE x)
{
    protectedCall([&] {
        StringValue(x);
    });
    return QString::fromUtf8(RSTRING_PTR(x), RSTRING_LEN(x));
}

template <>
QByteArray fromRuby<QByteArray>(VALUE x)
{
    protectedCall([&] {
        StringValue(x);
    });
    return QByteArray(RSTRING_PTR(x), RSTRING_LEN(x));
}

template <typename T>
T qListLikeFromRuby(VALUE x)
{
    protectedCall([&] {
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
    using Value = typename T::value_type;
    protectedCall([&] {
        x = rb_check_hash_type(x);
    });
    T hash;
    protectedCall([&] {
        rb_hash_foreach(x, [](VALUE key, VALUE value, VALUE arg) {
            auto &hash = *reinterpret_cast<T *>(arg);
            hash[fromRuby<Key>(key)] = fromRuby<Value>(value);
        }, reinterpret_cast<VALUE>(&hash));
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
    return protectedCall([&] {
        auto at = rb_funcall(x, "to_r", 0);
        int num = RRATIONAL(at)->num;
        int den = RRATIONAL(at)->den;
        return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
    });
}

template <>
QVariant fromVariant<QVariant>(VALUE x)
{
}

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
TypeCategory rubyValueCategory(VALUE x);

QVariant fromRuby(VALUE x, int type);

} // namespace RubyQml
