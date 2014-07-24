#include "ext_pointer.h"
#include "markable.h"
#include "objectdata.h"
#include "objectgc.h"
#include "kernel.h"
#include <QObject>
#include <QHash>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QCoreApplication>

namespace RubyQml {

Ext_Pointer::Ext_Pointer(RubyValue self) :
    self(self)
{
}

Ext_Pointer::~Ext_Pointer()
{
    ObjectGC::instance()->debug() << "\u267B\uFE0F  releasing object:" << mObject << "managed:" << mIsManaged;
}

RubyValue Ext_Pointer::fromQObject(QObject *obj, bool owned)
{
    auto klass = wrapperRubyClass<Ext_Pointer>();
    auto ptr = klass.newInstance();
    klass.unwrap(ptr)->setQObject(obj, owned);
    return ptr;
}

QObject *Ext_Pointer::fetchQObject()
{
    if (!mObject) {
        fail("QML::QtObjectError", "referencing already deleted Qt Object");
    }
    return mObject;
}

void Ext_Pointer::setQObject(QObject *obj, bool managed)
{
    if (!obj) {
        throw std::logic_error("null object");
    }
    mObject = obj;
    ObjectGC::instance()->addObject(obj);

    preferManaged(managed);
    ObjectGC::instance()->debug() << "\u2728  acquiring object:" << mObject;
}

void Ext_Pointer::setManaged(bool managed)
{
    if (mObject) {
        if (managed) {
            QQmlEngine::setObjectOwnership(mObject, QQmlEngine::JavaScriptOwnership);
            mJSValue = Kernel::engine()->newQObject(mObject);
        } else {
            QQmlEngine::setObjectOwnership(mObject, QQmlEngine::CppOwnership);
            mJSValue = QJSValue();
        }

        ObjectData::getOrCreate(mObject)->owned = managed;
        mIsManaged = managed;
    }
}

void Ext_Pointer::preferManaged(bool managed)
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

RubyValue Ext_Pointer::ext_isManaged() const
{
    return RubyValue::from(mIsManaged);
}

RubyValue Ext_Pointer::ext_setManaged(RubyValue managed)
{
    setManaged(managed.to<bool>());
    return ext_isManaged();
}

RubyValue Ext_Pointer::ext_preferManaged(RubyValue managed)
{
    preferManaged(managed.to<bool>());
    return ext_isManaged();
}

RubyValue Ext_Pointer::ext_isNull() const
{
    return RubyValue::from(!mObject);
}

RubyValue Ext_Pointer::ext_toString() const
{
    QString name;
    QDebug(&name) << mObject.data();
    return RubyValue::from(name);
}

void Ext_Pointer::gc_mark()
{
    if (mIsManaged) {
        ObjectGC::instance()->markOwnedObject(mObject);
    }
}

void Ext_Pointer::defineClass()
{
    WrapperRubyClass<Ext_Pointer> klass(RubyModule::fromPath("QML"), "Pointer");
    klass.defineMethod("managed?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_Pointer::ext_isManaged));
    klass.defineMethod("managed=", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_Pointer::ext_setManaged));
    klass.defineMethod("prefer_managed", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_Pointer::ext_preferManaged));
    klass.defineMethod("null?", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_Pointer::ext_isNull));
    klass.defineMethod("to_s", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_Pointer::ext_toString));
    klass.aliasMethod("inspect", "to_s");
}

} // namespace RubyQml
