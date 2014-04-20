#include "conversion.h"
#include "objectbase.h"
#include "metaobject.h"
#include <ruby/intern.h>

namespace RubyQml {


namespace detail {

VALUE Conversion<const char *>::to(const char *str)
{
    return protect([&] {
        return rb_enc_str_new_cstr(str, rb_utf8_encoding());
    });
}

QByteArray Conversion<QByteArray>::from(VALUE x)
{
    protect([&] {
        StringValue(x);
    });
    return QByteArray(RSTRING_PTR(x), RSTRING_LEN(x));
}

VALUE Conversion<QByteArray>::to(const QByteArray &str)
{
    return toRuby(str.constData());
}

QString Conversion<QString>::from(VALUE x)
{
    protect([&] {
        StringValue(x);
    });
    return QString::fromUtf8(RSTRING_PTR(x), RSTRING_LEN(x));
}

VALUE Conversion<QString>::to(const QString &str)
{
    return toRuby(str.toUtf8().constData());
}

QDateTime Conversion<QDateTime>::from(VALUE x)
{
    return protect([&] {
        auto at = rb_funcall(x, rb_intern("to_r"), 0);
        int num = RRATIONAL(at)->num;
        int den = RRATIONAL(at)->den;
        return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
    });
}

VALUE Conversion<QDateTime>::to(const QDateTime &dateTime)
{
    return protect([&] {
        auto at = rb_rational_new(INT2FIX(dateTime.toMSecsSinceEpoch()), INT2FIX(1000));
        return rb_funcall(rb_cTime, rb_intern("at"), at);
    });
}

namespace {

struct ConverterHash
{
    using FromRuby = QVariant (*)(VALUE);
    using ToRuby = VALUE (*)(const QVariant &);

    ConverterHash()
    {
        addFromRuby<char>();
        addFromRuby<short>();
        addFromRuby<long>();
        addFromRuby<long long>();
        addFromRuby<unsigned char>();
        addFromRuby<unsigned short>();
        addFromRuby<unsigned long>();
        addFromRuby<unsigned long long>();

        addFromRuby<int>();
        addFromRuby<unsigned int>();

        addFromRuby<float>();
        addFromRuby<double>();

        addFromRuby<QString>();
        addFromRuby<QByteArray>();

        addFromRuby<QVariant>();
        addFromRuby<QVariantList>();
        addFromRuby<QVariantHash>();
        addFromRuby<QVariantMap>();

        addFromRuby<QDateTime>();

        addFromRuby<QObject *>();

        addToRuby<char>();
        addToRuby<short>();
        addToRuby<long>();
        addToRuby<long long>();

        addToRuby<unsigned char>();
        addToRuby<unsigned short>();
        addToRuby<unsigned long>();
        addToRuby<unsigned long long>();

        addToRuby<float>();
        addToRuby<double>();

        addToRuby<QByteArray>();
        addToRuby<QString>();

        addToRuby<QVariant>();
        addToRuby<QVariantList>();
        addToRuby<QVariantHash>();
        addToRuby<QVariantMap>();

        addToRuby<QDateTime>();
        addToRuby<QObject *>();
    }

    template <typename T>
    void addFromRuby()
    {
        fromRubyHash[qMetaTypeId<T>()] = [](VALUE value) {
            return QVariant::fromValue(fromRuby<T>(value));
        };
    }
    template <typename T>
    void addToRuby()
    {
        toRubyHash[qMetaTypeId<T>()] = [](const QVariant &variant) {
            return toRuby(variant.value<T>());
        };
    }

    QHash<int, FromRuby> fromRubyHash;
    QHash<int, ToRuby> toRubyHash;
};

Q_GLOBAL_STATIC(ConverterHash, converterHash)

} // unnamed namespace

QVariant Conversion<QVariant>::from(VALUE x)
{
    return fromRuby(x, -1);
}

VALUE Conversion<QVariant>::to(const QVariant &variant)
{
    return converterHash->toRubyHash[variant.userType()](variant);
}

QObject *Conversion<QObject *>::from(VALUE x)
{
    return fromRuby<ObjectBase *>(x)->qObject();
}

VALUE Conversion<QObject *>::to(QObject *obj)
{
    auto klass = send(MetaObject::rubyClass(), "object_class");
    auto value = send(klass, "new");
    fromRuby<ObjectBase *>(value)->setQObject(obj);
    return value;
}

} // namespace detail

TypeCategory metaTypeToCategory(int metaType)
{
    switch (metaType) {

    case QMetaType::Char:
    case QMetaType::Short:
    case QMetaType::Long:
    case QMetaType::LongLong:

    case QMetaType::UChar:
    case QMetaType::UShort:
    case QMetaType::ULong:
    case QMetaType::ULongLong:

    case QMetaType::Int:
    case QMetaType::UInt:
        return TypeCategory::Integer;

    case QMetaType::Float:
    case QMetaType::Double:
        return TypeCategory::Float;

    case QMetaType::QByteArray:
    case QMetaType::QString:
        return TypeCategory::String;

    case QMetaType::QVariantList:
        return TypeCategory::Array;

    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
        return TypeCategory::Hash;

    case QMetaType::QDateTime:
        return TypeCategory::Time;

    case QMetaType::QObjectStar:
        return TypeCategory::QtObject;

    default:
        return TypeCategory::Invalid;
    }
}

int categoryToMetaType(TypeCategory category)
{
    switch (category) {
    case TypeCategory::Integer:
        return QMetaType::Int;
    case TypeCategory::Float:
        return QMetaType::Double;
    case TypeCategory::String:
        return QMetaType::QString;
    case TypeCategory::Array:
        return QMetaType::QVariantList;
    case TypeCategory::Hash:
        return QMetaType::QVariantHash;
    case TypeCategory::Time:
        return QMetaType::QDateTime;
    case TypeCategory::QtObject:
        return QMetaType::QObjectStar;
    default:
        return QMetaType::UnknownType;
    }
}

TypeCategory rubyValueCategory(VALUE x)
{
    return protect([&] {
        switch (BUILTIN_TYPE(x)) {
        case T_FIXNUM:
        case T_BIGNUM:
            return TypeCategory::Integer;
        case T_FLOAT:
            return TypeCategory::Float;
        case T_STRING:
            return TypeCategory::String;
        case T_ARRAY:
            return TypeCategory::Array;
        case T_HASH:
            return TypeCategory::Hash;
        default:
            break;
        }
        if (rb_obj_is_kind_of(x, rb_cTime)) {
            return TypeCategory::Time;
        }
        if (rb_obj_is_kind_of(x, ObjectBase::rubyClass())) {
            return TypeCategory::QtObject;
        }
        return TypeCategory::Invalid;
    });
}

QVariant fromRuby(VALUE x, int type)
{
    if (type < 0) {
        type = categoryToMetaType(rubyValueCategory(x));
    }
    return detail::converterHash->fromRubyHash[type](x);
}

ID idFromValue(VALUE sym)
{
    ID id;
    protect([&] {
        sym = rb_convert_type(sym, T_SYMBOL, "Symbol", "to_sym");
        id = SYM2ID(sym);
    });
    return id;
}

} // namespace RubyQml
