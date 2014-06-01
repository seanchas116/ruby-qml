#include "ext_objectpointer.h"
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>

namespace RubyQml {
namespace Ext {

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

ObjectPointer::ObjectPointer()
{
}

ObjectPointer::~ObjectPointer()
{
    if (mHasOwnership) {
        destroyObject(mObject);
    }
}

QObject *ObjectPointer::qObject()
{
    if (!mObject) {
        fail("QML::NullObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void ObjectPointer::setQObject(QObject *obj, bool hasOwnership)
{
    if (mHasOwnership && mObject) {
        destroyObject(mObject);
    }
    mObject = obj;
    mHasOwnership = hasOwnership;
}

void ObjectPointer::setOwnership(bool ownership)
{
    if (mObject) {
        mHasOwnership = ownership;
    }
}

VALUE ObjectPointer::hasOwnership() const
{
    return toRuby(mHasOwnership);
}

VALUE ObjectPointer::withOwnership() const
{
    auto other = send(rubyClass(), "new");
    ObjectPointer::getPointer(other)->setOwnership(true);
    return other;
}

VALUE ObjectPointer::isNull() const
{
    return toRuby(bool(mObject));
}

VALUE ObjectPointer::toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return toRuby(name);
}

VALUE ObjectPointer::mObjectBaseClass = Qnil;

ObjectPointer::ClassBuilder ObjectPointer::buildClass()
{
    protect([&] {
        mObjectBaseClass = rb_define_class_under(rb_path2class("QML"), "ObjectBase", rb_cObject);
    });
    ClassBuilder builder("QML", "ObjectPointer");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectPointer::hasOwnership)>("has_ownership?");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectPointer::withOwnership)>("with_ownership");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectPointer::isNull)>("null?");
    builder.defineMethod<METHOD_TYPE_NAME(&ObjectPointer::toString)>("to_s");
    builder.aliasMethod("to_s", "inspect");
    return builder;
}

} // namespace Ext
} // namespace RubyQml
