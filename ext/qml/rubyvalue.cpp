#include "rubyvalue.h"
#include "util.h"
#include "ext_pointer.h"
#include "ext_metaobject.h"
#include "objectdata.h"
#include "accessobject.h"
#include "listmodel.h"
#include "ext_accesssupport.h"
#include "rubyclasses.h"
#include <ruby/intern.h>
#define ONIG_ESCAPE_UCHAR_COLLISION
#include <ruby/encoding.h>
#include <QVariant>
#include <QDateTime>
#include <QRect>
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
        add<QDate>();

        add<QPoint>();
        add<QPointF>();
        add<QSize>();
        add<QSizeF>();
        add<QRect>();
        add<QRectF>();

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

template <> QByteArray Conversion<QByteArray>::from(RubyValue x)
{
    x = convertToString(x);
    return QByteArray(RSTRING_PTR(VALUE(x)), RSTRING_LEN(VALUE(x)));
}

template <> RubyValue Conversion<QByteArray>::to(const QByteArray &str)
{
    return RubyValue::from(str.constData());
}

template <> QString Conversion<QString>::from(RubyValue x)
{
    x = convertToString(x);
    return QString::fromUtf8(RSTRING_PTR(VALUE(x)), RSTRING_LEN(VALUE(x)));
}

template <> RubyValue Conversion<QString>::to(const QString &str)
{
    return RubyValue::from(str.toUtf8().constData());
}

template <> QDateTime Conversion<QDateTime>::from(RubyValue x)
{
    long long num;
    long long den;
    protect([&] {
        x = rb_funcall(x, rb_intern("to_time"), 0);
        auto at = rb_funcall(x, rb_intern("to_r"), 0);
        num = NUM2LL(RRATIONAL(at)->num);
        den = NUM2LL(RRATIONAL(at)->den);
    });
    return QDateTime::fromMSecsSinceEpoch(num * 1000 / den);
}

template <> RubyValue Conversion<QDateTime>::to(const QDateTime &dateTime) {
    RubyValue ret;
    protect([&] {
        auto at = rb_rational_new(LL2NUM(dateTime.toMSecsSinceEpoch()), INT2FIX(1000));
        ret = rb_funcall(rb_cTime, rb_intern("at"), 1, at);
        ret = rb_funcall(ret, rb_intern("to_datetime"), 0);
    });
    return ret;
}

template <> QDate Conversion<QDate>::from(RubyValue x)
{
    auto y = x.send("year").to<int>();
    auto m = x.send("month").to<int>();
    auto d = x.send("day").to<int>();
    return QDate(y, m, d);
}

template <> RubyValue Conversion<QDate>::to(const QDate &date)
{
    return RubyValue::fromPath("Date").send("new",
                                            RubyValue::from(date.year()),
                                            RubyValue::from(date.month()),
                                            RubyValue::from(date.day()));
}

template <> QPoint Conversion<QPoint>::from(RubyValue value)
{
    return QPoint(value.send("x").to<int>(), value.send("y").to<int>());
}

template <> RubyValue Conversion<QPoint>::to(const QPoint &point)
{
    return RubyValue::fromPath("QML::Geometry::Point")
        .send("new", RubyValue::from(point.x()), RubyValue::from(point.y()));
}

template <> QPointF Conversion<QPointF>::from(RubyValue value)
{
    return QPoint(value.send("x").to<double>(), value.send("y").to<double>());
}

template <> RubyValue Conversion<QPointF>::to(const QPointF &point)
{
    return RubyValue::fromPath("QML::Geometry::Point")
        .send("new", RubyValue::from(point.x()), RubyValue::from(point.y()));
}

template <> QSize Conversion<QSize>::from(RubyValue value)
{
    return QSize(value.send("width").to<int>(), value.send("height").to<int>());
}

template <> RubyValue Conversion<QSize>::to(const QSize &size)
{
    return RubyValue::fromPath("QML::Geometry::Size")
        .send("new", RubyValue::from(size.width()), RubyValue::from(size.height()));
}

template <> QSizeF Conversion<QSizeF>::from(RubyValue value)
{
    return QSize(value.send("width").to<double>(), value.send("height").to<double>());
}

template <> RubyValue Conversion<QSizeF>::to(const QSizeF &size)
{
    return RubyValue::fromPath("QML::Geometry::Size")
        .send("new", RubyValue::from(size.width()), RubyValue::from(size.height()));
}

template <> QRect Conversion<QRect>::from(RubyValue value)
{
    auto x = value.send("x").to<int>();
    auto y = value.send("y").to<int>();
    auto w = value.send("width").to<int>();
    auto h = value.send("height").to<int>();
    return QRect(QPoint(x, y), QSize(w, h));
}

template <> RubyValue Conversion<QRect>::to(const QRect &rect)
{
    return RubyValue::fromPath("QML::Geometry::Rectangle")
        .send("new",
              RubyValue::from(rect.x()), RubyValue::from(rect.y()),
              RubyValue::from(rect.width()), RubyValue::from(rect.height()));
}

template <> QRectF Conversion<QRectF>::from(RubyValue value)
{
    auto x = value.send("x").to<double>();
    auto y = value.send("y").to<double>();
    auto w = value.send("width").to<double>();
    auto h = value.send("height").to<double>();
    return QRect(QPoint(x, y), QSize(w, h));
}

template <> RubyValue Conversion<QRectF>::to(const QRectF &rect)
{
    return RubyValue::fromPath("QML::Geometry::Rectangle")
        .send("new",
              RubyValue::from(rect.x()), RubyValue::from(rect.y()),
              RubyValue::from(rect.width()), RubyValue::from(rect.height()));
}

template <> QVariant Conversion<QVariant>::from(RubyValue x)
{
    return x.toVariant();
}

template <> RubyValue Conversion<QVariant>::to(const QVariant &variant)
{
    return RubyValue::fromVariant(variant);
}

template <> const QMetaObject *Conversion<const QMetaObject *>::from(RubyValue x)
{
    if (x == RubyValue()) {
        return nullptr;
    }
    return wrapperRubyClass<Ext::MetaObject>().unwrap(x)->metaObject();
}

template <> RubyValue Conversion<const QMetaObject *>::to(const QMetaObject *metaobj)
{
    if (!metaobj) {
        return Qnil;
    }
    return Ext::MetaObject::fromMetaObject(metaobj);
}

} // namespace detail

Q_GLOBAL_STATIC(QSet<int>, enumeratorMetaTypes)

RubyValue RubyValue::fromPath(const char *path)
{
    RubyValue ret;
    protect([&] {
        ret = rb_path2class(path);
    });
    return ret;
}

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
        static auto dateTimeClass = RubyValue::fromPath("DateTime");
        if (rb_obj_is_kind_of(x, dateTimeClass)) {
            return metaType == QMetaType::QDateTime;
        }
        static auto dateClass = RubyValue::fromPath("Date");
        if (rb_obj_is_kind_of(x, dateClass)) {
            return metaType == QMetaType::QDate || metaType == QMetaType::QDateTime;
        }

        if (rb_obj_is_kind_of(x, rubyClasses().wrapper)) {
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
        if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext::MetaObject>().toValue())) {
            return metaType == QMetaType::type("const QMetaObject*");
        }
        if (rb_obj_is_kind_of(x, rubyClasses().access)) {
            return metaType == QMetaType::QObjectStar;
        }

        static auto pointClass = RubyValue::fromPath("QML::Geometry::Point");
        if (rb_obj_is_kind_of(x, pointClass)) {
            return metaType == QMetaType::QPoint || metaType == QMetaType::QPointF;
        }
        static auto sizeClass = RubyValue::fromPath("QML::Geometry::Size");
        if (rb_obj_is_kind_of(x, sizeClass)) {
            return metaType == QMetaType::QSize || metaType == QMetaType::QSizeF;
        }
        static auto rectClass = RubyValue::fromPath("QML::Geometry::Rectangle");
        if (rb_obj_is_kind_of(x, rectClass)) {
            return metaType == QMetaType::QRect || metaType == QMetaType::QRectF;
        }
        static auto listModelClass = RubyValue::fromPath("QML::Data::ListModel");

        if (rb_obj_is_kind_of(x, listModelClass)) {
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
        return QMetaType::QString;
    case T_STRING:
        if (rb_enc_get_index(x)  == rb_ascii8bit_encindex()) {
            return QMetaType::QByteArray;
        } else {
            return QMetaType::QString;
        }
    case T_ARRAY:
        return QMetaType::QVariantList;
    case T_HASH:
        return QMetaType::QVariantHash;
    default:
        break;
    }
    auto get = [&]() -> int {
        if (rb_obj_is_kind_of(x, rb_cTime)) {
            return QMetaType::QDateTime;
        }
        static auto dateTimeClass = RubyValue::fromPath("DateTime");
        if (rb_obj_is_kind_of(x, dateTimeClass)) {
            return QMetaType::QDateTime;
        }
        static auto dateClass = RubyValue::fromPath("Date");
        if (rb_obj_is_kind_of(x, dateClass)) {
            return QMetaType::QDate;
        }

        if (rb_obj_is_kind_of(x, rubyClasses().wrapper)) {
            return QMetaType::QObjectStar;
        }
        if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext::MetaObject>().toValue())) {
            return QMetaType::type("const QMetaObject*");
        }
        if (rb_obj_is_kind_of(x, rubyClasses().access)) {
            return QMetaType::QObjectStar;
        }

        static auto pointClass = RubyValue::fromPath("QML::Geometry::Point");
        if (rb_obj_is_kind_of(x, pointClass)) {
            return QMetaType::QPointF;
        }
        static auto sizeClass = RubyValue::fromPath("QML::Geometry::Size");
        if (rb_obj_is_kind_of(x, sizeClass)) {
            return QMetaType::QSizeF;
        }
        static auto rectClass = RubyValue::fromPath("QML::Geometry::Rectangle");
        if (rb_obj_is_kind_of(x, rectClass)) {
            return QMetaType::QRectF;
        }

        static auto listModelClass = RubyValue::fromPath("QML::Data::ListModel");
        if (rb_obj_is_kind_of(x, listModelClass)) {
            return  QMetaType::QObjectStar;
        }
        return QMetaType::UnknownType;
    };
    int result;
    protect([&] {
        result = get();
    });
    return result;
}

RubyValue RubyValue::fromVariant(const QVariant &variant)
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

RubyValue RubyValue::fromQObject(QObject *obj, bool implicit)
{
    if (!obj) {
        return Qnil;
    }

    if (implicit) {
        auto accessObj = dynamic_cast<AccessObject *>(obj);
        if (accessObj) {
            return accessObj->wrappedValue();
        }
        auto listModel = dynamic_cast<ListModel *>(obj);
        if (listModel) {
            return listModel->rubyModel();
        }
    }

    auto data = ObjectData::getOrCreate(obj);
    if (data->wrapper) {
        return data->wrapper;
    }

    auto metaobj = Ext::MetaObject::fromMetaObject(obj->metaObject());
    auto pointer = Ext::Pointer::fromQObject(obj, false);
    auto wrapper = metaobj.send("build_class").send("allocate");
    wrapper.send("pointer=", pointer);
    wrapper.send("initialize");

    data->wrapper = wrapper;
    return wrapper;
}

QObject *RubyValue::toQObject() const
{
    auto x = *this;

    if (x == RubyValue()) {
        return nullptr;
    }

    if (x.isKindOf(rubyClasses().access)) {
        auto support = x.send("class").send("access_support");
        return wrapperRubyClass<Ext::AccessSupport>().unwrap(support)->wrap(x);
    }
    auto listModelClass = RubyValue::fromPath("QML::Data::ListModel");
    if (x.isKindOf(listModelClass)) {
        return new ListModel(x);
    }

    if (!x.isKindOf(rubyClasses().wrapper)) {
        fail("QML::ConversionError",
             QString("expected QML::Wrapper , got %1")
                .arg(x.send("class").send("name").to<QString>()));
    }
    auto objptr = x.send("pointer");
    auto obj = wrapperRubyClass<Ext::Pointer>().unwrap(objptr)->fetchQObject();
    Ext::MetaObject::fromMetaObject(obj->metaObject()).send("build_class");
    return obj;
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
