#include <QVariant>
#include <QDateTime>
#include <QDebug>

namespace {

template <typename THashLike>
void getHashLike(const QVariant &variant, void (*callback)(const char *, QVariant *))
{
    auto hash = variant.value<THashLike>();
    for (auto i = hash.begin(); i != hash.end(); ++i) {
        callback(i.key().toUtf8().data(), new QVariant(i.value()));
    }
}

}

extern "C" {

const char *rbqml_metatype_name(int typeNum)
{
    return QMetaType::typeName(typeNum);
}

QVariant *rbqml_variant_new()
{
    return new QVariant();
}

QVariant *rbqml_variant_from_int(int value)
{
    return new QVariant(value);
}

QVariant *rbqml_variant_from_float(double value)
{
    return new QVariant(value);
}

QVariant *rbqml_variant_from_boolean(int value)
{
    return new QVariant(bool(value));
}

QVariant *rbqml_variant_from_string(const char *str)
{
    return new QVariant(QString(str));
}

QVariant *rbqml_variant_from_qobject(QObject *obj)
{
    return new QVariant(QVariant::fromValue(obj));
}

QVariant *rbqml_variant_from_voidp(void *ptr)
{
    return new QVariant(QVariant::fromValue(ptr));
}

struct VariantArray
{
    int count;
    const QVariant **variants;
};

QVariant *rbqml_variant_from_array(VariantArray array)
{
    QVariantList variantList;
    variantList.reserve(array.count);
    for (int i = 0; i < array.count; ++i) {
        variantList << *array.variants[i];
    }
    return new QVariant(variantList);
}

struct StringVariantArray
{
    int count;
    const char **keys;
    const QVariant **variants;
};

QVariant *rbqml_variant_from_hash(StringVariantArray array)
{
    QVariantHash variantHash;
    variantHash.reserve(array.count);
    for (int i = 0; i < array.count; ++i) {
        variantHash[array.keys[i]] = *array.variants[i];
    }
    return new QVariant(variantHash);
}

QVariant *rbqml_variant_from_time(int year, int month, int day, int hour, int minute, int second, int msecond, int gmtOffset)
{
    QDateTime dateTime(QDate(year, month, day), QTime(hour, minute, second, msecond));
    dateTime.setUtcOffset(gmtOffset);
    return new QVariant(dateTime);
}

int rbqml_variant_to_int(const QVariant *variant)
{
    return variant->toInt();
}

double rbqml_variant_to_float(const QVariant *variant)
{
    return variant->toDouble();
}

QObject *rbqml_variant_to_qobject(const QVariant *variant)
{
    return variant->value<QObject *>();
}

void *rbqml_variant_to_voidp(const QVariant *variant)
{
    return variant->value<void *>();
}

QVariant *rbqml_variant_unnest(const QVariant *variant)
{
    auto v = *variant;
    while (true) {
        if (v.userType() != QMetaType::QVariant) {
            return new QVariant(v);
        }
        v = v.value<QVariant>();
    }
}

void rbqml_variant_get_string(const QVariant *variant, void (*callback)(const char *))
{
    switch (variant->userType()) {
    case QMetaType::QString:
        callback(variant->toString().toUtf8().data());
        break;
    case QMetaType::QByteArray:
        callback(variant->toByteArray().data());
        break;
    default:
        break;
    }
}

void rbqml_variant_get_array(const QVariant *variant, void (*callback)(QVariant *))
{
    for (const auto &x : variant->toList()) {
        callback(new QVariant(x));
    }
}

void rbqml_variant_get_hash(const QVariant *variant, void (*callback)(const char *, QVariant *))
{
    switch (variant->userType()) {
    case QMetaType::QVariantHash:
        getHashLike<QVariantHash>(*variant, callback);
        break;
    case QMetaType::QVariantMap:
        getHashLike<QVariantMap>(*variant, callback);
        break;
    default:
        break;
    }
}

void rbqml_variant_get_time(const QVariant *variant, int *out)
{
    auto dateTime = variant->toDateTime();
    auto date = dateTime.date();
    auto time = dateTime.time();
    out[0] = date.year();
    out[1] = date.month();
    out[2] = date.day();
    out[3] = time.hour();
    out[4] = time.minute();
    out[5] = time.second();
    out[6] = time.msec();
    out[7] = dateTime.utcOffset();
}

int rbqml_variant_type(const QVariant *variant)
{
    return variant->userType();
}

QVariant *rbqml_variant_convert(const QVariant *variant, int type)
{
    if (type == QMetaType::QVariant) {
        return new QVariant(QMetaType::QVariant, variant);
    }

    QVariant result = *variant;

    if (!result.convert(type)) {
        return new QVariant();
    }
    return new QVariant(result);
}

int rbqml_variant_is_valid(const QVariant *variant)
{
    return variant->isValid();
}

QVariant *rbqml_variant_dup(const QVariant *variant)
{
    return new QVariant(*variant);
}

void rbqml_variant_destroy(QVariant *variant)
{
    delete variant;
}

}
