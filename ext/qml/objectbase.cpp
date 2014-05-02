#include "objectbase.h"
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>

namespace RubyQml {

namespace {

void destroyObject(QObject *obj)
{
    // owned by JS
    if (QQmlEngine::objectOwnership(obj) == QQmlEngine::JavaScriptOwnership) {
        return;
    }
    // owned by parent
    if (obj->parent()) {
        return;
    }

    obj->deleteLater();
}

}

ObjectBase::ObjectBase()
{
}

ObjectBase::~ObjectBase()
{
    if (mHasOwnership) {
        destroyObject(mObject);
    }
}

QObject *ObjectBase::qObject()
{
    if (!mObject) {
        fail("QML::NullObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void ObjectBase::setQObject(QObject *obj, bool hasOwnership)
{
    mObject = obj;
    mHasOwnership = hasOwnership;
}

void ObjectBase::setOwnership(bool ownership)
{
    if (mObject) {
        mHasOwnership = ownership;
    }
}

VALUE ObjectBase::hasOwnership() const
{
    return toRuby(mHasOwnership);
}

VALUE ObjectBase::withOwnership() const
{
    auto other = send(rubyClass(), "new");
    ObjectBase::getPointer(other)->setOwnership(true);
    return other;
}

VALUE ObjectBase::isNull() const
{
    return toRuby(bool(mObject));
}

VALUE ObjectBase::toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return toRuby(name);
}

ObjectBase::ClassBuilder ObjectBase::buildClass()
{
    ClassBuilder builder("QML", "ObjectBase");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectBase::hasOwnership)>("has_ownership?");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectBase::withOwnership)>("with_ownership");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectBase::isNull)>("null?");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectBase::toString)>("to_s");
    builder.aliasMethod("to_s", "inspect");
    return builder;
}

} // namespace RubyQml
