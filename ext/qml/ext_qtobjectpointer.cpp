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
    return toRuby(bool(mObject));
}

VALUE QtObjectPointer::toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return toRuby(name);
}

VALUE QtObjectPointer::mObjectBaseClass = Qnil;

QtObjectPointer::ClassBuilder QtObjectPointer::buildClass()
{
    protect([&] {
        mObjectBaseClass = rb_define_class_under(rb_path2class("QML"), "QtObjectBase", rb_cObject);
    });
    ClassBuilder builder("QML", "QtObjectPointer");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::hasOwnership)>("has_ownership?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::withOwnership)>("with_ownership");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::isNull)>("null?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::toString)>("to_s");
    builder.aliasMethod("to_s", "inspect");
    return builder;
}

} // namespace Ext
} // namespace RubyQml
