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
    void setQObject(QObject *obj, bool managed);
    bool isManaged() const { return mIsManaged; }
    void setManaged(bool managed);
    void preferManaged(bool managed);

    RubyValue ext_isManaged() const;
    RubyValue ext_setManaged(RubyValue managed);
    RubyValue ext_preferManaged(RubyValue managed);
    RubyValue ext_isNull() const;
    RubyValue ext_toString() const;

    void gc_mark();

    static void defineClass();

private:
    const RubyValue self;

    bool mIsManaged = false;
    QPointer<QObject> mObject;
    QJSValue mJSValue;
};

} // namespace Ext
} // namespace RubyQml
