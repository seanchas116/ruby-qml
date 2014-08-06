#include "rubyvalue.h"
#include "conversionerror.h"
#include "util.h"
#include "ext_pointer.h"
#include "ext_metaobject.h"
#include "objectdata.h"
#include "accessobject.h"
#include "listmodel.h"
#include "ext_accesswrapperfactory.h"
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
    if (rb_type(x) == T_SYMBOL) {
        x = rb_sym_to_s(x);
    }
    return rb_convert_type(x, T_STRING, "String", "to_str");
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
    return rb_enc_str_new(str, strlen(str), rb_utf8_encoding());
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

template <> QDateTime Conversion<QDateTime>::from(RubyValue time)
{
    int offset;

    if (time.isKindOf(rb_cTime)) {
        offset = time.send(RUBYQML_INTERN("gmt_offset")).to<int>();
    } else { // DateTime
        VALUE dayOffset = time.send(RUBYQML_INTERN("offset"));
        offset = RubyValue(RRATIONAL(dayOffset)->num).to<int>() * 24 * 60 * 60 / RubyValue(RRATIONAL(dayOffset)->den).to<int>();
        time = time.send(RUBYQML_INTERN("to_time"));
    }
    timeval at = rb_time_timeval(time);

    QDateTime dateTime;
    dateTime.setOffsetFromUtc(offset);
    dateTime.setMSecsSinceEpoch(at.tv_sec * 1000 + at.tv_usec / 1000);
    return dateTime;
}

template <> RubyValue Conversion<QDateTime>::to(const QDateTime &dateTime)
{
    RubyValue sec = rb_rational_new(RubyValue::from(dateTime.toMSecsSinceEpoch()), RubyValue::from(1000));
    RubyValue time = rb_time_num_new(sec, RubyValue::from(dateTime.offsetFromUtc()));
    return time.send(RUBYQML_INTERN("to_datetime"));
}

template <> QDate Conversion<QDate>::from(RubyValue x)
{
    int y = x.send(RUBYQML_INTERN("year")).to<int>();
    int m = x.send(RUBYQML_INTERN("month")).to<int>();
    int d = x.send(RUBYQML_INTERN("day")).to<int>();
    return QDate(y, m, d);
}

template <> RubyValue Conversion<QDate>::to(const QDate &date)
{
    static auto klass = RubyClass::fromPath("Date");
    return klass.toValue().send(RUBYQML_INTERN("new"), RubyValue::from(date.year()), RubyValue::from(date.month()), RubyValue::from(date.day()));
}

template <> QPoint Conversion<QPoint>::from(RubyValue value)
{
    auto x = value.send(RUBYQML_INTERN("x")).to<int>();
    auto y = value.send(RUBYQML_INTERN("y")).to<int>();
    return QPoint(x, y);
}

template <> RubyValue Conversion<QPoint>::to(const QPoint &point)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Point");
    return klass.toValue().send(RUBYQML_INTERN("new"), RubyValue::from(point.x()), RubyValue::from(point.y()));
}

template <> QPointF Conversion<QPointF>::from(RubyValue value)
{
    auto x = value.send(RUBYQML_INTERN("x")).to<double>();
    auto y = value.send(RUBYQML_INTERN("y")).to<double>();
    return QPointF(x, y);
}

template <> RubyValue Conversion<QPointF>::to(const QPointF &point)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Point");
    return klass.toValue().send(RUBYQML_INTERN("new"), RubyValue::from(point.x()), RubyValue::from(point.y()));
}

template <> QSize Conversion<QSize>::from(RubyValue value)
{
    auto width = value.send(RUBYQML_INTERN("width")).to<int>();
    auto height= value.send(RUBYQML_INTERN("height")).to<int>();
    return QSize(width, height);
}

template <> RubyValue Conversion<QSize>::to(const QSize &size)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Size");
    return klass.toValue().send(RUBYQML_INTERN("new"), RubyValue::from(size.width()), RubyValue::from(size.height()));
}

template <> QSizeF Conversion<QSizeF>::from(RubyValue value)
{
    auto width = value.send(RUBYQML_INTERN("width")).to<double>();
    auto height = value.send(RUBYQML_INTERN("height")).to<double>();
    return QSizeF(width, height);
}

template <> RubyValue Conversion<QSizeF>::to(const QSizeF &size)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Size");
    return klass.toValue().send(RUBYQML_INTERN("new"), RubyValue::from(size.width()), RubyValue::from(size.height()));
}

template <> QRect Conversion<QRect>::from(RubyValue value)
{
    auto x = value.send(RUBYQML_INTERN("x")).to<int>();
    auto y = value.send(RUBYQML_INTERN("y")).to<int>();
    auto w = value.send(RUBYQML_INTERN("width")).to<int>();
    auto h = value.send(RUBYQML_INTERN("height")).to<int>();
    return QRect(QPoint(x, y), QSize(w, h));
}

template <> RubyValue Conversion<QRect>::to(const QRect &rect)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Rectangle");
    return klass.toValue().send(RUBYQML_INTERN("new"),
                                RubyValue::from(rect.x()), RubyValue::from(rect.y()),
                                RubyValue::from(rect.width()), RubyValue::from(rect.height()));
}

template <> QRectF Conversion<QRectF>::from(RubyValue value)
{
    auto x = value.send(RUBYQML_INTERN("x")).to<double>();
    auto y = value.send(RUBYQML_INTERN("y")).to<double>();
    auto w = value.send(RUBYQML_INTERN("width")).to<double>();
    auto h = value.send(RUBYQML_INTERN("height")).to<double>();
    return QRectF(QPointF(x, y), QSizeF(w, h));
}

template <> RubyValue Conversion<QRectF>::to(const QRectF &rect)
{
    static auto klass = RubyClass::fromPath("QML::Geometry::Rectangle");
    return klass.toValue().send(RUBYQML_INTERN("new"),
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
    return wrapperRubyClass<Ext_MetaObject>().unwrap(x)->metaObject();
}

template <> RubyValue Conversion<const QMetaObject *>::to(const QMetaObject *metaobj)
{
    if (!metaobj) {
        return Qnil;
    }
    return Ext_MetaObject::fromMetaObject(metaobj);
}

} // namespace detail

Q_GLOBAL_STATIC(QSet<int>, enumeratorMetaTypes)

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

    if (rb_obj_is_kind_of(x, rb_cTime)) {
        return metaType == QMetaType::QDateTime;
    }
    static auto dateTimeClass = RubyClass::fromPath("DateTime");
    if (rb_obj_is_kind_of(x, dateTimeClass)) {
        return metaType == QMetaType::QDateTime;
    }
    static auto dateClass = RubyClass::fromPath("Date");
    if (rb_obj_is_kind_of(x, dateClass)) {
        return metaType == QMetaType::QDate || metaType == QMetaType::QDateTime;
    }
    static auto objectBaseClass = RubyClass::fromPath("QML::QtObjectBase");
    if (rb_obj_is_kind_of(x, objectBaseClass)) {
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
    if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext_MetaObject>())) {
        return metaType == QMetaType::type("const QMetaObject*");
    }
    auto accessModule = RubyModule::fromPath("QML::Access");
    if (rb_obj_is_kind_of(x, accessModule)) {
        return metaType == QMetaType::QObjectStar;
    }

    static auto pointClass = RubyModule::fromPath("QML::Geometry::Point");
    if (rb_obj_is_kind_of(x, pointClass)) {
        return metaType == QMetaType::QPoint || metaType == QMetaType::QPointF;
    }
    static auto sizeClass = RubyModule::fromPath("QML::Geometry::Size");
    if (rb_obj_is_kind_of(x, sizeClass)) {
        return metaType == QMetaType::QSize || metaType == QMetaType::QSizeF;
    }
    static auto rectClass = RubyModule::fromPath("QML::Geometry::Rectangle");
    if (rb_obj_is_kind_of(x, rectClass)) {
        return metaType == QMetaType::QRect || metaType == QMetaType::QRectF;
    }
    static auto listModelClass = RubyModule::fromPath("QML::Data::ListModel");

    if (rb_obj_is_kind_of(x, listModelClass)) {
        return metaType == QMetaType::QObjectStar;
    }
    return false;
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
    if (rb_obj_is_kind_of(x, rb_cTime)) {
        return QMetaType::QDateTime;
    }
    static auto dateTimeClass = RubyModule::fromPath("DateTime");
    if (rb_obj_is_kind_of(x, dateTimeClass)) {
        return QMetaType::QDateTime;
    }
    static auto dateClass = RubyModule::fromPath("Date");
    if (rb_obj_is_kind_of(x, dateClass)) {
        return QMetaType::QDate;
    }

    static auto objectBaseClass = RubyClass::fromPath("QML::QtObjectBase");
    if (rb_obj_is_kind_of(x, objectBaseClass)) {
        return QMetaType::QObjectStar;
    }
    if (rb_obj_is_kind_of(x, wrapperRubyClass<Ext_MetaObject>())) {
        return QMetaType::type("const QMetaObject*");
    }
    auto accessModule = RubyModule::fromPath("QML::Access");
    if (rb_obj_is_kind_of(x, accessModule)) {
        return QMetaType::QObjectStar;
    }

    static auto pointClass = RubyModule::fromPath("QML::Geometry::Point");
    if (rb_obj_is_kind_of(x, pointClass)) {
        return QMetaType::QPointF;
    }
    static auto sizeClass = RubyModule::fromPath("QML::Geometry::Size");
    if (rb_obj_is_kind_of(x, sizeClass)) {
        return QMetaType::QSizeF;
    }
    static auto rectClass = RubyModule::fromPath("QML::Geometry::Rectangle");
    if (rb_obj_is_kind_of(x, rectClass)) {
        return QMetaType::QRectF;
    }

    static auto listModelClass = RubyModule::fromPath("QML::Data::ListModel");
    if (rb_obj_is_kind_of(x, listModelClass)) {
        return  QMetaType::QObjectStar;
    }
    return QMetaType::UnknownType;
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
        throw ConversionError(QString("failed to convert QVariant value (%1)")
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
            throw ConversionError(QString("failed to convert QObject value (%1 to %2)")
                                  .arg(qobj->metaObject()->className())
                                  .arg(metaobj->className()));
        }
        if (enumeratorMetaTypes->contains(type)) {
            auto intValue = to<int>();
            auto data = QMetaType::create(type);
            *static_cast<int *>(data) = intValue;
            return QVariant(type, data);
        }
        throw ConversionError(QString("failed to convert Ruby value (%1 to %2)")
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
        auto accessObj = dynamic_cast<AccessWrapper *>(obj);
        if (accessObj) {
            return accessObj->wrappedValue();
        }
        auto listModel = dynamic_cast<ListModel *>(obj);
        if (listModel) {
            return listModel->rubyModel();
        }
    }

    auto data = ObjectData::getOrCreate(obj);
    if (data->rubyObject) {
        return data->rubyObject;
    }

    auto metaobj = Ext_MetaObject::fromMetaObject(obj->metaObject());
    auto pointer = Ext_Pointer::fromQObject(obj, false);

    RubyValue wrapper;

    auto klass = rb_funcall(metaobj, RUBYQML_INTERN("build_class"), 0);
    wrapper = rb_obj_alloc(klass);
    rb_funcall(wrapper, RUBYQML_INTERN("pointer="), 1, pointer);
    rb_obj_call_init(wrapper, 0, nullptr);

    data->rubyObject = wrapper;
    return wrapper;
}

QObject *RubyValue::toQObject() const
{
    auto x = *this;

    if (x == RubyValue()) {
        return nullptr;
    }
    static auto accessModule = RubyModule::fromPath("QML::Access");
    if (x.isKindOf(accessModule)) {
        auto wrapperFactory = rb_funcall(rb_obj_class(x), RUBYQML_INTERN("access_wrapper_factory"), 0);
        return wrapperRubyClass<Ext_AccessWrapperFactory>().unwrap(wrapperFactory)->create(x);
    }
    static auto listModelClass = RubyModule::fromPath("QML::Data::ListModel");
    if (x.isKindOf(listModelClass)) {
        return new ListModel(x);
    }

    static auto objectBaseClass = RubyClass::fromPath("QML::QtObjectBase");
    if (!x.isKindOf(objectBaseClass)) {
        throw ConversionError(QString("expected QML::QtObjectbase, got %1")
                              .arg(x.send("class").send("name").to<QString>()));
    }
    auto objptr = x.send(RUBYQML_INTERN("pointer"));
    auto obj = wrapperRubyClass<Ext_Pointer>().unwrap(objptr)->fetchQObject();
    Ext_MetaObject::fromMetaObject(obj->metaObject()).send(RUBYQML_INTERN("build_class"));
    return obj;
}

ID RubyValue::toID() const
{
    return SYM2ID(rb_convert_type(*this, T_SYMBOL, "Symbol", "to_sym"));
}

void RubyValue::addEnumeratorMetaType(int metaType)
{
    *enumeratorMetaTypes << metaType;
}

} // namespace RubyQml
