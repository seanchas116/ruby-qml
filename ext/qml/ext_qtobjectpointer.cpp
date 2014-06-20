#include "ext_qtobjectpointer.h"
#include "markable.h"
#include "objectdata.h"
#include "objectgc.h"
#include <QObject>
#include <QHash>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QCoreApplication>

namespace RubyQml {
namespace Ext {

QtObjectPointer::QtObjectPointer()
{
}

QtObjectPointer::~QtObjectPointer()
{
    if (mIsOwned) {
        destroy();
    }
}

RubyValue QtObjectPointer::fromQObject(QObject *obj, bool owned)
{
    auto ptr = newAsRuby();
    QtObjectPointer::getPointer(ptr)->setQObject(obj, owned);
    return ptr;
}

QObject *QtObjectPointer::fetchQObject()
{
    if (!mObject) {
        fail("QML::QtObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void QtObjectPointer::setQObject(QObject *obj, bool owned)
{
    if (!obj) {
        throw std::logic_error("null object");
    }
    if (mIsOwned) {
        destroy();
    }
    auto context = QQmlEngine::contextForObject(obj);
    if (context) {
        QQmlEngine::setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);
        mJSValue = context->engine()->newQObject(obj);
        mIsOwned = false;
    }
    mObject = obj;
    ObjectGC::instance()->addObject(obj);
    setOwned(owned);
}

void QtObjectPointer::setOwned(bool owned)
{
    if (mObject) {
        ObjectData::getOrCreate(mObject)->owned = owned;
        mIsOwned = owned;
    }
}

void QtObjectPointer::destroy()
{
    if (!mIsOwned) {
        fail("QML::QtObjectError", "destroying non-owned Qt object");
    }
    setOwned(false);

    if (mObject && !mObject->parent() && !qobject_cast<QCoreApplication *>(mObject)) {
        delete mObject;
    }
}

RubyValue QtObjectPointer::ext_isOwned() const
{
    return RubyValue::from(mIsOwned);
}

RubyValue QtObjectPointer::ext_setOwned(RubyValue owned)
{
    setOwned(owned.to<bool>());
    return ext_isOwned();
}

RubyValue QtObjectPointer::ext_isNull() const
{
    return RubyValue::from(!mObject);
}

RubyValue QtObjectPointer::ext_toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return RubyValue::from(name);
}

RubyValue QtObjectPointer::ext_destroy()
{
    destroy();
    return self();
}

void QtObjectPointer::mark()
{
    if (mIsOwned) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

RubyValue QtObjectPointer::mObjectBaseClass = Qnil;

void QtObjectPointer::initClass()
{
    protect([&] {
        mObjectBaseClass = rb_define_class_under(rb_path2class("QML"), "QtObjectBase", rb_cObject);
    });
    ClassBuilder builder("QML", "QtObjectPointer");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::ext_isOwned)>("owned?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::ext_setOwned)>("owned=");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::ext_isNull)>("null?");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::ext_toString)>("to_s");
    builder.defineMethod<METHOD_TYPE_NAME(&QtObjectPointer::ext_destroy)>("destroy!");
    builder.aliasMethod("to_s", "inspect");
}

} // namespace Ext
} // namespace RubyQml
