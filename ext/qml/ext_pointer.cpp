#include "ext_pointer.h"
#include "markable.h"
#include "objectdata.h"
#include "objectgc.h"
#include "application.h"
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
    ObjectGC::instance()->debug() << "\u267B\uFE0F  releasing object:" << mObject << "managed:" << mIsManaged;
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

void Pointer::setQObject(QObject *obj, bool managed)
{
    if (!obj) {
        throw std::logic_error("null object");
    }
    mObject = obj;
    ObjectGC::instance()->addObject(obj);

    preferManaged(managed);
    ObjectGC::instance()->debug() << "\u2728  acquiring object:" << mObject;
}

void Pointer::setManaged(bool managed)
{
    if (mObject) {
        if (managed) {
            QQmlEngine::setObjectOwnership(mObject, QQmlEngine::JavaScriptOwnership);
            mJSValue = Application::engine()->newQObject(mObject);
        } else {
            QQmlEngine::setObjectOwnership(mObject, QQmlEngine::CppOwnership);
            mJSValue = QJSValue();
        }

        ObjectData::getOrCreate(mObject)->owned = managed;
        mIsManaged = managed;
    }
}

void Pointer::preferManaged(bool managed)
{
    auto ownership = QQmlEngine::objectOwnership(mObject);

    if (ownership == QQmlEngine::JavaScriptOwnership) {
        managed = true;
    } else {
        // owned by parent
        if (mObject->parent()) {
            managed = false;
        }
    }

    setManaged(managed);
}

RubyValue Pointer::ext_isManaged() const
{
    return RubyValue::from(mIsManaged);
}

RubyValue Pointer::ext_setManaged(RubyValue managed)
{
    setManaged(managed.to<bool>());
    return ext_isManaged();
}

RubyValue Pointer::ext_preferManaged(RubyValue managed)
{
    preferManaged(managed.to<bool>());
    return ext_isManaged();
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

void Pointer::gc_mark()
{
    if (mIsManaged) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

void Pointer::defineClass()
{
    WrapperRubyClass<Pointer> klass(RubyModule::fromPath("QML"), "Pointer");
    klass.defineMethod("managed?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isManaged));
    klass.defineMethod("managed=", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_setManaged));
    klass.defineMethod("prefer_managed", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_preferManaged));
    klass.defineMethod("null?", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_isNull));
    klass.defineMethod("to_s", RUBYQML_MEMBER_FUNCTION_INFO(&Pointer::ext_toString));
    klass.aliasMethod("inspect", "to_s");
}

} // namespace Ext
} // namespace RubyQml
