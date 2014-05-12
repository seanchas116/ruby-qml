#include "conversion.h"
#include "objectpointer.h"
#include "metaobject.h"
#include <ruby/intern.h>
#include <QtCore/QDebug>

namespace RubyQml {


namespace detail {

VALUE Conversion<const char *>::to(const char *str)
{
    VALUE ret;
    protect([&] {
        ret = rb_enc_str_new_cstr(str, rb_utf8_encoding());
    });
    return ret;
}

namespace {

VALUE convertToString(VALUE x)
{
    VALUE ret;
    protect([&] {
        if (rb_type(x) == T_SYMBOL) {
            x = rb_sym_to_s(x);
        }
        ret = rb_convert_type(x, T_STRING, "String", "to_str");
    });
    return ret;
}

}

QByteArray Conversion<QByteArray>::from(VALUE x)
{
    x = convertToString(x);
    return QByteArray(RSTRING_PTR(x), RSTRING_LEN(x));
}

VALUE Conversion<QByteArray>::to(const QByteArray &str)
{
    return toRuby(str.constData());
}

QString Conversion<QString>::from(VALUE x)
{
    x = convertToString(x);
    return QString::fromUtf8(RSTRING_PTR(x), RSTRING_LEN(x));
}

VALUE Conversion<QString>::to(const QString &str)
{
    return toRuby(str.toUtf8().constData());
}

QDateTime Conversion<QDateTime>::from(VALUE x)
{
    QDateTime ret;
    protect([&] {
        auto at = rb_convert_type(x, T_RATIONAL, "Rational", "to_r");
        long long num = NUM2LL(RRATIONAL(at)->num);
        long long den = NUM2LL(RRATIONAL(at)->den);
        ret = QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
    });
    return ret;
}

VALUE Conversion<QDateTime>::to(const QDateTime &dateTime)
{
    VALUE ret;
    protect([&] {
        auto at = rb_rational_new(LL2NUM(dateTime.toMSecsSinceEpoch()), INT2FIX(1000));
        ret = rb_funcall(rb_cTime, rb_intern("at"), 1, at);
    });
    return ret;
}

namespace {

struct ConverterHash
{
    using FromRuby = QVariant (*)(VALUE);
    using ToRuby = VALUE (*)(const QVariant &);

    ConverterHash()
    {
        add<bool>();

        add<char>();
        add<short>();
        add<long>();
        add<long long>();
        add<unsigned char>();
        add<unsigned short>();
        add<unsigned long>();
        add<unsigned long long>();

        add<int>();
        add<unsigned int>();

        add<float>();
        add<double>();

        add<QString>();
        add<QByteArray>();

        add<QVariant>();
        add<QVariantList>();
        add<QVariantHash>();
        add<QVariantMap>();

        add<QDateTime>();

        add<QObject *>();
        add<const QMetaObject *>();

        fromRubyHash[QMetaType::UnknownType] = [](VALUE value) {
            return QVariant();
        };
        toRubyHash[QMetaType::UnknownType] = [](const QVariant &variant) {
            return Qnil;
        };
    }

    template <typename T>
    void add()
    {
        addFromRuby<T>();
        addToRuby<T>();
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
    auto &hash = converterHash->toRubyHash;
    auto type = variant.userType();
    if (!hash.contains(type)) {
        qWarning() << __PRETTY_FUNCTION__ << ": unsupported meta type" << type;
        return Qnil;
    }
    return hash[type](variant);
}

QObject *Conversion<QObject *>::from(VALUE x)
{
    VALUE objptr;
    protect([&] {
        if (!rb_obj_is_kind_of(x, ObjectPointer::objectBaseClass())) {
            rb_raise(rb_eTypeError, "expected QML::ObjectBase, got %s", rb_obj_classname(x));
        }
        objptr = rb_ivar_get(x, rb_intern("@objptr"));
    });
    return ObjectPointer::getPointer(objptr)->qObject();
}

VALUE Conversion<QObject *>::to(QObject *obj)
{
    auto metaObject = MetaObject::fromObject(obj);

    auto objptr = ObjectPointer::newAsRuby();
    ObjectPointer::getPointer(objptr)->setQObject(obj);

    return send(send(metaObject, "object_class"), "new", objptr);
}

const QMetaObject *Conversion<const QMetaObject *>::from(VALUE x)
{
    return MetaObject::getPointer(x)->metaObject();
}

VALUE Conversion<const QMetaObject *>::to(const QMetaObject *metaobj)
{
    auto value = MetaObject::newAsRuby();
    MetaObject::getPointer(value)->setMetaObject(metaobj);
    return value;
}

} // namespace detail

TypeCategory metaTypeToCategory(int metaType)
{
    switch (metaType) {

    case QMetaType::Bool:
        return TypeCategory::Boolean;

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
        if (metaType == QMetaType::type("const QMetaObject*")) {
            return TypeCategory::QtMetaObject;
        }
        return TypeCategory::Invalid;
    }
}

int categoryToMetaType(TypeCategory category)
{
    switch (category) {
    case TypeCategory::Boolean:
        return QMetaType::Bool;
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
    case TypeCategory::QtMetaObject:
        return QMetaType::type("const QMetaObject*");
    default:
        return QMetaType::UnknownType;
    }
}

TypeCategory rubyValueCategory(VALUE x)
{
    auto objectBaseClass = ObjectPointer::rubyClass();
    auto metaObjectClass = MetaObject::rubyClass();
    TypeCategory category;
    protect([&] {
        switch (rb_type(x)) {
        case T_TRUE:
        case T_FALSE:
            category = TypeCategory::Boolean;
            return;
        case T_FIXNUM:
        case T_BIGNUM:
            category = TypeCategory::Integer;
            return;
        case T_FLOAT:
            category = TypeCategory::Float;
            return;
        case T_SYMBOL:
        case T_STRING:
            category = TypeCategory::String;
            return;
        case T_ARRAY:
            category = TypeCategory::Array;
            return;
        case T_HASH:
            category = TypeCategory::Hash;
            return;
        default:
            break;
        }
        if (rb_obj_is_kind_of(x, rb_cTime)) {
            category = TypeCategory::Time;
            return;
        }
        if (rb_obj_is_kind_of(x, objectBaseClass)) {
            category = TypeCategory::QtObject;
            return;
        }
        if (rb_obj_is_kind_of(x, metaObjectClass)) {
            category = TypeCategory::QtMetaObject;
            return;
        }
        category = TypeCategory::Invalid;
    });
    return category;
}

QVariant fromRuby(VALUE x, int type)
{
    if (type < 0) {
        type = categoryToMetaType(rubyValueCategory(x));
    }
    auto &hash = detail::converterHash->fromRubyHash;
    if (!hash.contains(type)) {
        qWarning() << __PRETTY_FUNCTION__ << ": unsupported Ruby type" << type;
        return QVariant();
    }
    return hash[type](x);
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

VALUE echoConversion(VALUE value)
{
    auto variant = fromRuby<QVariant>(value);
    return toRuby(variant);
}

} // namespace RubyQml
