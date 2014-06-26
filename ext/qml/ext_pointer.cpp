#include "ext_pointer.h"
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

Pointer::Pointer(RubyValue self) :
    self(self)
{
}

Pointer::~Pointer()
{
    if (mIsOwned) {
        destroy();
    }
}

RubyValue Pointer::fromQObject(QObject *obj, bool owned)
{
    auto klass = wrapperRubyClass<Pointer>();
    auto ptr = klass.newInstance();
    klass.unwrap(ptr)->setQObject(obj, owned);
    return ptr;
}

QObject *Pointer::fetchQObject()
{
    if (!mObject) {
        fail("QML::QtObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void Pointer::setQObject(QObject *obj, bool owned)
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

void Pointer::setOwned(bool owned)
{
    if (mObject) {
        ObjectData::getOrCreate(mObject)->owned = owned;
        mIsOwned = owned;
    }
}

void Pointer::destroy()
{
    if (!mIsOwned) {
        fail("QML::QtObjectError", "destroying non-owned Qt object");
    }
    setOwned(false);

    if (mObject && !mObject->parent() && !qobject_cast<QCoreApplication *>(mObject)) {
        delete mObject;
    }
}

RubyValue Pointer::ext_isOwned() const
{
    return RubyValue::from(mIsOwned);
}

RubyValue Pointer::ext_setOwned(RubyValue owned)
{
    setOwned(owned.to<bool>());
    return ext_isOwned();
}

RubyValue Pointer::ext_isNull() const
{
    return RubyValue::from(!mObject);
}

RubyValue Pointer::ext_toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return RubyValue::from(name);
}

RubyValue Pointer::ext_destroy()
{
    destroy();
    return self;
}

void Pointer::gc_mark()
{
    if (mIsOwned) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

void Pointer::defineClass()
{
    WrapperRubyClass<Pointer> klass("QML", "Pointer");
    klass.defineMethod("owned?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isOwned));
    klass.defineMethod("owned=", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_setOwned));
    klass.defineMethod("null?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isNull));
    klass.defineMethod("to_s", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_toString));
    klass.defineMethod("destroy!", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_destroy));
    klass.aliasMethod("to_s", "inspect");
}

} // namespace Ext
} // namespace RubyQml
