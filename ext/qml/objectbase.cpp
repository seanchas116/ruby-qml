#include "objectbase.h"
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>

namespace RubyQml {

namespace {

auto refCountProperty = "rubyqml_refcount";

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
    fromRuby<ObjectBase *>(other)->setOwnership(true);
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

ObjectBase::Definition ObjectBase::createDefinition()
{
    Definition def("QML", "ObjectBase");
    def.defineMethod<METHOD_TYPE_NAME(&ObjectBase::hasOwnership)>("has_ownership?");
    def.defineMethod<METHOD_TYPE_NAME(&ObjectBase::withOwnership)>("with_ownership");
    def.defineMethod<METHOD_TYPE_NAME(&ObjectBase::isNull)>("null?");
    def.defineMethod<METHOD_TYPE_NAME(&ObjectBase::toString)>("to_s");
    def.aliasMethod("to_s", "inspect");
    return def;
}

} // namespace RubyQml
