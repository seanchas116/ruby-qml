#include "foreignclass.h"
#include "foreignclass_metaobject.h"
#include "foreignclass_object.h"
#include <QDebug>

namespace RubyQml {

ForeignClass::ForeignClass(const SP<ForeignClass> &superclass) :
    mSuperclass(superclass)
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

void ForeignClass::createMetaObject()
{
    mMetaObject = makeSP<MetaObject>(shared_from_this());
}

SP<ForeignClass::MetaObject> ForeignClass::metaObject()
{
    if (!mMetaObject) {
        throw std::runtime_error("meta object has not been created");
    }
    return mMetaObject;
}

void ForeignClass::emitSignal(Object *obj, std::size_t id, const QVariantList &args)
{
    auto metaobj = metaObject();
    auto metamethod = metaobj->method(metaobj->signalIndexHash()[id] + metaobj->methodOffset());
    qDebug() << "emitting signal" << metamethod.name();
    if (metamethod.parameterCount() != args.size()) {
        qWarning() << "wrong number of signal arguments";
        return;
    }
    QVariantList argsToPass = args;
    while (argsToPass.size() < 10) {
        argsToPass << QVariant();
    }
    metamethod.invoke(obj,
                      Q_ARG(QVariant, argsToPass[0]),
                      Q_ARG(QVariant, argsToPass[1]),
                      Q_ARG(QVariant, argsToPass[2]),
                      Q_ARG(QVariant, argsToPass[3]),
                      Q_ARG(QVariant, argsToPass[4]),
                      Q_ARG(QVariant, argsToPass[5]),
                      Q_ARG(QVariant, argsToPass[6]),
                      Q_ARG(QVariant, argsToPass[7]),
                      Q_ARG(QVariant, argsToPass[8]),
                      Q_ARG(QVariant, argsToPass[9]));
}

QVariant ForeignClass::callMethod(Object *obj, size_t id, const QVariantList &args)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    Q_UNUSED(args);
    return QVariant();
}

void ForeignClass::setProperty(Object *obj, size_t id, const QVariant &value)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    Q_UNUSED(value);
}

QVariant ForeignClass::getProperty(Object *obj, size_t id)
{
    Q_UNUSED(obj);
    Q_UNUSED(id);
    return QVariant();
}

void ForeignClass::addMethod(const QByteArray &name, std::size_t id, const QList<QByteArray> &params, Method::Access access, Method::Type type)
{
    if (mMetaObject) {
        qWarning() << "meta object alread created";
        return;
    }
    Method method = { .name = name, .params = params, .access = access, .type = type, .id = id };
    mMethods << method;
}

void ForeignClass::addSignal(const QByteArray &name, std::size_t id, const QList<QByteArray> &params)
{
    addMethod(name, id, params, Method::Access::Protected, Method::Type::Signal);
}

void ForeignClass::addProperty(const QByteArray &name, std::size_t getterId, std::size_t setterId, Property::Flags flags, bool hasNotifySignal,std::size_t notifySignalId)
{
    if (mMetaObject) {
        qWarning() << "meta object alread created";
        return;
    }
    Property property = { .name = name, .flags = flags, .setterId = setterId, .getterId = getterId, .hasNotifySignal = hasNotifySignal, .notifySignalId = notifySignalId};
    mProperties << property;
}

} // namespace RubyQml
