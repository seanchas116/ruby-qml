#include "ext_qtobjectpointer.h"
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

QtObjectPointer::QtObjectPointer()
{
}

QtObjectPointer::~QtObjectPointer()
{
    if (mHasOwnership) {
        destroyObject(mObject);
    }
}

VALUE QtObjectPointer::fromQObject(QObject *obj, bool hasOwnership)
{
    auto ptr = newAsRuby();
    QtObjectPointer::getPointer(ptr)->setQObject(obj, hasOwnership);
    return ptr;
}

QObject *QtObjectPointer::fetchQObject()
{
    if (!mObject) {
        fail("QML::NullObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void QtObjectPointer::setQObject(QObject *obj, bool hasOwnership)
{
    if (mHasOwnership && mObject) {
        destroyObject(mObject);
    }
    mObject = obj;
    mHasOwnership = hasOwnership;
}

void QtObjectPointer::setOwnership(bool ownership)
{
    if (mObject) {
        mHasOwnership = ownership;
    }
}

VALUE QtObjectPointer::hasOwnership() const
{
    return toRuby(mHasOwnership);
}

VALUE QtObjectPointer::withOwnership() const
{
    auto other = send(rubyClass(), "new");
    QtObjectPointer::getPointer(other)->setOwnership(true);
    return other;
}

VALUE QtObjectPointer::isNull() const
{
    return toRuby(!mObject);
}

VALUE QtObjectPointer::toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return toRuby(name);
}

VALUE QtObjectPointer::destroy()
{
    destroyObject(mObject);
    mHasOwnership = false;
    return self();
}

VALUE QtObjectPointer::mObjectBaseClass = Qnil;

void QtObjectPointer::initClass()
{
    protect([&] {
        mObjectBaseClass = rb_define_class_under(rb_path2class("QML"), "QtObjectBase", rb_cObject);
    });
    ClassBuilder builder("QML", "QtObjectPointer");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::hasOwnership)>("has_ownership?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::withOwnership)>("with_ownership");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::isNull)>("null?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::toString)>("to_s");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::destroy)>("destroy!");
    builder.aliasMethod("to_s", "inspect");
}

} // namespace Ext
} // namespace RubyQml
