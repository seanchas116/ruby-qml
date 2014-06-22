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

QtObjectPointer::QtObjectPointer(RubyValue self) :
    self(self)
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
    auto klass = wrapperRubyClass<QtObjectPointer>();
    auto ptr = klass.newInstance();
    klass.unwrap(ptr)->setQObject(obj, owned);
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
    return self;
}

void QtObjectPointer::gc_mark()
{
    if (mIsOwned) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

RubyClass QtObjectPointer::mObjectBaseClass;

void QtObjectPointer::defineClass()
{
    mObjectBaseClass = RubyClass("QML", "QtObjectBase");

    WrapperRubyClass<QtObjectPointer> klass("QML", "QtObjectPointer");
    klass.defineMethod("owned?", RUBYQML_MEMBER_FUNCTION_INFO(&QtObjectPointer::ext_isOwned));
    klass.defineMethod("owned=", RUBYQML_MEMBER_FUNCTION_INFO(&QtObjectPointer::ext_setOwned));
    klass.defineMethod("null?", RUBYQML_MEMBER_FUNCTION_INFO(&QtObjectPointer::ext_isNull));
    klass.defineMethod("to_s", RUBYQML_MEMBER_FUNCTION_INFO(&QtObjectPointer::ext_toString));
    klass.defineMethod("destroy!", RUBYQML_MEMBER_FUNCTION_INFO(&QtObjectPointer::ext_destroy));
    klass.aliasMethod("to_s", "inspect");
}

} // namespace Ext
} // namespace RubyQml
