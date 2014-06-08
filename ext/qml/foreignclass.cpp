#include "foreignclass.h"
#include "foreignobject.h"
#include <QDebug>

namespace RubyQml {

ForeignClass::ForeignClass()
{
}

ForeignClass::~ForeignClass()
{
}

QList<ForeignClass::Method> ForeignClass::signalMethods() const
{
    QList<Method> sigs;
    std::copy_if(mMethods.begin(), mMethods.end(), std::back_inserter(sigs), [](const Method &m) {
        return m.type == Method::Type::Signal;
    });
    return sigs;
}

QList<ForeignClass::Method> ForeignClass::nonSignalMethods() const
{
    QList<Method> sigs;
    std::copy_if(mMethods.begin(), mMethods.end(), std::back_inserter(sigs), [](const Method &m) {
        return m.type != Method::Type::Signal;
    });
    return sigs;
}

QVariant ForeignClass::callMethod(ForeignObject *obj, size_t id, const QVariantList &args)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    Q_UNUSED(args);
    return QVariant();
}

void ForeignClass::setProperty(ForeignObject *obj, size_t id, const QVariant &value)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    Q_UNUSED(value);
}

QVariant ForeignClass::getProperty(ForeignObject *obj, size_t id)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    return QVariant();
}

void ForeignClass::addMethod(const QByteArray &name, std::size_t id, const QList<QByteArray> &params, Method::Access access, Method::Type type)
{
    Method method = { .name = name, .params = params, .access = access, .type = type, .id = id };
    mMethods << method;
}

void ForeignClass::addSignal(const QByteArray &name, std::size_t id, const QList<QByteArray> &params)
{
    addMethod(name, id, params, Method::Access::Protected, Method::Type::Signal);
}

void ForeignClass::addProperty(const QByteArray &name, std::size_t getterId, std::size_t setterId, Property::Flags flags, bool hasNotifySignal,std::size_t notifySignalId)
{
    Property property = { .name = name, .flags = flags, .setterId = setterId, .getterId = getterId, .hasNotifySignal = hasNotifySignal, .notifySignalId = notifySignalId};
    mProperties << property;
}

} // namespace RubyQml
