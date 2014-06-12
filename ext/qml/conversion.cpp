#include "conversion.h"
#include "ext_qtobjectpointer.h"
#include "ext_metaobject.h"
#include "objectdata.h"
#include "accessobject.h"
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
    long long num;
    long long den;
    protect([&] {
        auto at = rb_funcall(x, rb_intern("to_r"), 0);
        num = NUM2LL(RRATIONAL(at)->num);
        den = NUM2LL(RRATIONAL(at)->den);
    });
    return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
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
            Q_UNUSED(value);
            return QVariant();
        };
        toRubyHash[QMetaType::UnknownType] = [](const QVariant &variant) {
            Q_UNUSED(variant);
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
    return fromRuby(x, defaultMetaTypeFor(x));
}

VALUE Conversion<QVariant>::to(const QVariant &variant)
{
    auto &hash = converterHash->toRubyHash;
    auto type = variant.userType();
    if (QMetaType::metaObjectForType(type)) {
        type = QMetaType::QObjectStar;
    }
    if (!hash.contains(type)) {
        fail("QML::ConversionError",
             QString("failed to convert QVariant value (%1)")
             .arg(QMetaType::typeName(type)));
    }
    return hash[type](variant);
}

QObject *Conversion<QObject *>::from(VALUE x)
{
    if (x == Qnil) {
        return nullptr;
    }

    bool isAccess;
    protect([&] {
        isAccess = rb_obj_is_kind_of(x, rb_path2class("QML::Access"));
    });
    if (isAccess) {
        VALUE accessptr;
        protect([&] {
            accessptr = rb_funcall(x, rb_intern("access_object"), 0);
        });
        return Ext::QtObjectPointer::getPointer(accessptr)->fetchQObject();
    }

    VALUE objptr;
    protect([&] {
        if (!rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass())) {
            rb_raise(rb_path2class("QML::ConversionError"), "expected QML::ObjectBase, got %s", rb_obj_classname(x));
        }
        objptr = rb_funcall(x, rb_intern("object_pointer"), 0);
    });
    auto obj = Ext::QtObjectPointer::getPointer(objptr)->fetchQObject();
    Ext::MetaObject::getPointer(Ext::MetaObject::fromMetaObject(obj->metaObject()))->buildRubyClass();
    return obj;
}

VALUE Conversion<QObject *>::to(QObject *obj)
{
    if (!obj) {
        return Qnil;
    }
    auto accessObj = dynamic_cast<AccessObject *>(obj);
    if (accessObj) {
        return accessObj->value();
    }

    auto data = ObjectData::get(obj);
    if (data) {
        return data->rubyObject();
    }

    auto metaobj = Ext::MetaObject::fromMetaObject(obj->metaObject());
    auto objptr = Ext::QtObjectPointer::fromQObject(obj, false);
    auto rubyobj = send(Ext::MetaObject::getPointer(metaobj)->buildRubyClass(), "allocate");
    send(rubyobj, "object_pointer=", objptr);
    send(rubyobj, "initialize");

    ObjectData::set(obj, std::make_shared<ObjectData>(rubyobj));
    return rubyobj;
}

const QMetaObject *Conversion<const QMetaObject *>::from(VALUE x)
{
    if (x == Qnil) {
        return nullptr;
    }
    return Ext::MetaObject::getPointer(x)->metaObject();
}

VALUE Conversion<const QMetaObject *>::to(const QMetaObject *metaobj)
{
    if (!metaobj) {
        return Qnil;
    }
    return Ext::MetaObject::fromMetaObject(metaobj);
}

} // namespace detail

bool convertibleTo(VALUE x, int metaType)
{
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
        if (rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass())) {
            if (metaType == QMetaType::QObjectStar) {
                return true;
            }
            if (QMetaType::metaObjectForType(metaType)) {
                auto metaObj = QMetaType::metaObjectForType(metaType);
                if (fromRuby<QObject *>(x)->inherits(metaObj->className())) {
                    return true;
                }
            }
            return false;
        }
        if (rb_obj_is_kind_of(x, Ext::MetaObject::rubyClass())) {
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

int defaultMetaTypeFor(VALUE x)
{
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
    if (rb_obj_is_kind_of(x, Ext::QtObjectPointer::objectBaseClass())) {
        return QMetaType::QObjectStar;
    }
    if (rb_obj_is_kind_of(x, Ext::MetaObject::rubyClass())) {
        return QMetaType::type("const QMetaObject*");
    }
    if (rb_obj_is_kind_of(x, rb_path2class("QML::Access"))) {
        return QMetaType::QObjectStar;
    }
    return QMetaType::UnknownType;
}

QVariant fromRuby(VALUE x, int type)
{
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
        fail("QML::ConversionError",
             QString("failed to convert Ruby value (%1 to %2)")
             .arg(rb_obj_classname(x))
             .arg(QMetaType::typeName(type)));
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
