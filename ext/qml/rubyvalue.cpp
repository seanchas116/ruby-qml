#include "rubyvalue.h"
#include "util.h"
#include "ext_qtobjectpointer.h"
#include "ext_metaobject.h"
#include "objectdata.h"
#include "accessobject.h"
#include <ruby/intern.h>
#define ONIG_ESCAPE_UCHAR_COLLISION
#include <ruby/encoding.h>
#include <QVariant>
#include <QDateTime>
#include <QSet>

namespace RubyQml {

namespace detail {

namespace {

RubyValue convertToString(RubyValue x)
{
    RubyValue ret;
    protect([&] {
        if (rb_type(x) == T_SYMBOL) {
            x = rb_sym_to_s(x);
        }
        ret = rb_convert_type(x, T_STRING, "String", "to_str");
    });
    return ret;
}

struct ConverterHash
{
    using FromRuby = QVariant (*)(RubyValue);
    using ToRuby = RubyValue (*)(const QVariant &);

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

        fromRubyHash[QMetaType::UnknownType] = [](RubyValue value) {
            Q_UNUSED(value);
            return QVariant();
        };
        toRubyHash[QMetaType::UnknownType] = [](const QVariant &variant) {
            Q_UNUSED(variant);
            return RubyValue();
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
        fromRubyHash[qMetaTypeId<T>()] = [](RubyValue value) {
            return QVariant::fromValue(value.to<T>());
        };
    }
    template <typename T>
    void addToRuby()
    {
        toRubyHash[qMetaTypeId<T>()] = [](const QVariant &variant) {
            return RubyValue::from(variant.value<T>());
        };
    }

    QHash<int, FromRuby> fromRubyHash;
    QHash<int, ToRuby> toRubyHash;
};

Q_GLOBAL_STATIC(ConverterHash, converterHash)

} // unnamed namespace

RubyValue Conversion<const char *>::to(const char *str)
{
    RubyValue ret;
    protect([&] {
        ret = rb_enc_str_new_cstr(str, rb_utf8_encoding());
    });
    return ret;
}

QByteArray Conversion<QByteArray>::from(RubyValue x)
{
    x = convertToString(x);
    return QByteArray(RSTRING_PTR(VALUE(x)), RSTRING_LEN(VALUE(x)));
}

RubyValue Conversion<QByteArray>::to(const QByteArray &str)
{
    return RubyValue::from(str.constData());
}

QString Conversion<QString>::from(RubyValue x)
{
    x = convertToString(x);
    return QString::fromUtf8(RSTRING_PTR(VALUE(x)), RSTRING_LEN(VALUE(x)));
}

RubyValue Conversion<QString>::to(const QString &str)
{
    return RubyValue::from(str.toUtf8().constData());
}

QDateTime Conversion<QDateTime>::from(RubyValue x)
{
    long long num;
    long long den;
    protect([&] {
        auto at = rb_funcall(x, rb_intern("to_r"), 0);
        num = NUM2LL(RRATIONAL(at)->num);
        den = NUM2LL(RRATIONAL(at)->den);
    });
    return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
}

RubyValue Conversion<QDateTime>::to(const QDateTime &dateTime)
{
    RubyValue ret;
    protect([&] {
        auto at = rb_rational_new(LL2NUM(dateTime.toMSecsSinceEpoch()), INT2FIX(1000));
        ret = rb_funcall(rb_cTime, rb_intern("at"), 1, at);
    });
    return ret;
}

QVariant Conversion<QVariant>::from(RubyValue x)
{
    return x.toVariant();
}

RubyValue Conversion<QVariant>::to(const QVariant &variant)
{
    return RubyValue::from(variant);
}

QObject *Conversion<QObject *>::from(RubyValue x)
{
    if (x == RubyValue()) {
        return nullptr;
    }

    bool isAccess;
    protect([&] {
        isAccess = rb_obj_is_kind_of(x, rb_path2class("QML::Access"));
    });
    if (isAccess) {
        RubyValue accessptr;
        protect([&] {
            accessptr = rb_funcall(x, rb_intern("access_object"), 0);
        });
        return wrapperRubyClass<Ext::QtObjectPointer>()->unwrap(accessptr)->fetchQObject();
    }

    RubyValue objptr;
    protect([&] {
        if (!rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass().toValue())) {
            rb_raise(rb_path2class("QML::ConversionError"), "expected QML::ObjectBase, got %s", rb_obj_classname(x));
        }
        objptr = rb_funcall(x, rb_intern("object_pointer"), 0);
    });
    auto obj = wrapperRubyClass<Ext::QtObjectPointer>()->unwrap(objptr)->fetchQObject();
    wrapperRubyClass<Ext::MetaObject>()->unwrap(Ext::MetaObject::fromMetaObject(obj->metaObject()))->buildRubyClass();
    return obj;
}

RubyValue Conversion<QObject *>::to(QObject *obj)
{
    if (!obj) {
        return Qnil;
    }
    auto accessObj = dynamic_cast<AccessObject *>(obj);
    if (accessObj) {
        return accessObj->value();
    }

    auto data = ObjectData::getOrCreate(obj);
    if (data->rubyObject) {
        return data->rubyObject;
    }

    auto metaobj = Ext::MetaObject::fromMetaObject(obj->metaObject());
    auto objptr = Ext::QtObjectPointer::fromQObject(obj, false);
    auto rubyobj = wrapperRubyClass<Ext::MetaObject>()->unwrap(metaobj)->buildRubyClass().send("allocate");
    rubyobj.send("object_pointer=", objptr);
    rubyobj.send("initialize");

    data->rubyObject = rubyobj;
    return rubyobj;
}

const QMetaObject *Conversion<const QMetaObject *>::from(RubyValue x)
{
    if (x == RubyValue()) {
        return nullptr;
    }
    return wrapperRubyClass<Ext::MetaObject>()->unwrap(x)->metaObject();
}

RubyValue Conversion<const QMetaObject *>::to(const QMetaObject *metaobj)
{
    if (!metaobj) {
        return Qnil;
    }
    return Ext::MetaObject::fromMetaObject(metaobj);
}

} // namespace detail

Q_GLOBAL_STATIC(QSet<int>, enumeratorMetaTypes)

bool RubyValue::isKindOf(RubyValue klass) const
{
    RubyValue result;
    protect([&] {
        result = rb_obj_is_kind_of(mValue, klass);
    });
    return result;
}

bool RubyValue::isConvertibleTo(int metaType) const
{
    auto x = *this;
    if (metaType == QMetaType::QVariant) {
        return true;
    }
    switch (rb_type(x)) {
    case T_NIL:
        return metaType == QMetaType::QObjectStar || metaType == QMetaType::type("const QMetaObject*");
    case T_TRUE:
    case T_FALSE:
        return metaType == QMetaType::Bool;
    case T_FIXNUM:
    case T_BIGNUM:
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

        case QMetaType::Float:
        case QMetaType::Double:
            return true;
        default:
            if (enumeratorMetaTypes->contains(metaType)) {
                return true;
            }
            return false;
        }
    case T_FLOAT:
        switch (metaType) {
        case QMetaType::Float:
        case QMetaType::Double:
            return true;
        default:
            return false;
        }
    case T_SYMBOL:
    case T_STRING:
        switch (metaType) {
        case QMetaType::QByteArray:
        case QMetaType::QString:
            return true;
        default:
            return false;
        }
    case T_ARRAY:
        switch (metaType) {
        case QMetaType::QVariantList:
            return true;
        default:
            return false;
        }
    case T_HASH:
        switch (metaType) {
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
            return true;
        default:
            return false;
        }
    default:

        break;
    }
    auto test = [&] {
        if (rb_obj_is_kind_of(x, rb_cTime)) {
            return metaType == QMetaType::QDateTime;
        }
        if (rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass().toValue())) {
            if (metaType == QMetaType::QObjectStar) {
                return true;
            }
            if (QMetaType::metaObjectForType(metaType)) {
                auto metaObj = QMetaType::metaObjectForType(metaType);
                if (x.to<QObject *>()->inherits(metaObj->className())) {
                    return true;
                }
            }
            return false;
        }
        if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext::MetaObject>()->toValue())) {
            return metaType == QMetaType::type("const QMetaObject*");
        }
        if (rb_obj_is_kind_of(x, rb_path2class("QML::Access"))) {
            return metaType == QMetaType::QObjectStar;
        }
        return false;
    };
    bool result;
    protect([&] {
        result = test();
    });
    return result;
}

int RubyValue::defaultMetaType() const
{
    auto x = *this;
    switch (rb_type(x)) {
    case T_NIL:
        return QMetaType::UnknownType;
    case T_TRUE:
    case T_FALSE:
        return QMetaType::Bool;
    case T_FIXNUM:
    case T_BIGNUM:
        return QMetaType::Int;
    case T_FLOAT:
        return QMetaType::Double;
    case T_SYMBOL:
    case T_STRING:
        return QMetaType::QString;
    case T_ARRAY:
        return QMetaType::QVariantList;
    case T_HASH:
        return QMetaType::QVariantHash;
    default:
        break;
    }
    if (rb_obj_is_kind_of(x, rb_cTime)) {
        return QMetaType::QDateTime;
    }
    if (rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass().toValue())) {
        return QMetaType::QObjectStar;
    }
    if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext::MetaObject>()->toValue())) {
        return QMetaType::type("const QMetaObject*");
    }
    if (rb_obj_is_kind_of(x, rb_path2class("QML::Access"))) {
        return QMetaType::QObjectStar;
    }
    return QMetaType::UnknownType;
}

RubyValue RubyValue::from(const QVariant &variant)
{
    auto &hash = detail::converterHash->toRubyHash;
    auto type = variant.userType();
    if (QMetaType::metaObjectForType(type)) {
        type = QMetaType::QObjectStar;
    }
    if (enumeratorMetaTypes->contains(type)) {
        auto intValue = *static_cast<const int *>(variant.data());
        return from(intValue);
    }
    if (!hash.contains(type)) {
        fail("QML::ConversionError",
             QString("failed to convert QVariant value (%1)")
             .arg(QMetaType::typeName(type)));
    }
    return hash[type](variant);
}

QVariant RubyValue::toVariant() const
{
    return toVariant(defaultMetaType());
}

QVariant RubyValue::toVariant(int type) const
{
    auto x = *this;
    auto &hash = detail::converterHash->fromRubyHash;
    if (!hash.contains(type)) {
        auto metaobj = QMetaType::metaObjectForType(type);
        if (metaobj) {
            auto qobj = hash[QMetaType::QObjectStar](x).value<QObject *>();
            if (qobj->inherits(metaobj->className())) {
                return QVariant::fromValue(qobj);
            }
            fail("QML::ConversionError",
                 QString("failed to convert QObject value (%1 to %2)")
                 .arg(qobj->metaObject()->className())
                 .arg(metaobj->className()));
        }
        if (enumeratorMetaTypes->contains(type)) {
            auto intValue = to<int>();
            auto data = QMetaType::create(type);
            *static_cast<int *>(data) = intValue;
            return QVariant(type, data);
        }
        fail("QML::ConversionError",
             QString("failed to convert Ruby value (%1 to %2)")
             .arg(rb_obj_classname(x))
             .arg(QMetaType::typeName(type)));
        return QVariant();
    }
    return hash[type](x);
}

ID RubyValue::toID() const
{
    ID id;
    protect([&] {
        id = SYM2ID(rb_convert_type(*this, T_SYMBOL, "Symbol", "to_sym"));
    });
    return id;
}

void RubyValue::addEnumeratorMetaType(int metaType)
{
    *enumeratorMetaTypes << metaType;
}

} // namespace RubyQml
