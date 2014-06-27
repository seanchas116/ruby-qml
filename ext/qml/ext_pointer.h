#pragma once
#include "rubyclass.h"
#include <QPointer>
#include <QJSValue>

namespace RubyQml {
namespace Ext {

class Pointer
{
public:
    Pointer(RubyValue self);
    ~Pointer();

    static RubyValue fromQObject(QObject *obj, bool owned);

    QObject *qObject() { return mObject; }
    QObject *fetchQObject();
    void setQObject(QObject *obj, bool owned);
    bool isOwned() const { return mIsOwned; }
    void setOwned(bool owned);

    RubyValue ext_isOwned() const;
    RubyValue ext_setOwned(RubyValue owned);
    RubyValue ext_isNull() const;
    RubyValue ext_toString() const;

    void gc_mark();

    static void defineClass();

private:
    const RubyValue self;

    bool mIsOwned = false;
    QPointer<QObject> mObject;
    QJSValue mJSValue;
};

} // namespace Ext
} // namespace RubyQml
