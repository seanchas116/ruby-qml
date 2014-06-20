#pragma once
#include "extbase.h"
#include <QPointer>
#include <QJSValue>

namespace RubyQml {
namespace Ext {

class QtObjectPointer : public ExtBase<QtObjectPointer>
{
    friend class ExtBase<QtObjectPointer>;

public:
    QtObjectPointer();
    ~QtObjectPointer();

    static RubyValue fromQObject(QObject *obj, bool owned);

    QObject *qObject() { return mObject; }
    QObject *fetchQObject();
    void setQObject(QObject *obj, bool owned);
    bool isOwned() const { return mIsOwned; }
    void setOwned(bool owned);
    void destroy();

    RubyValue ext_isOwned() const;
    RubyValue ext_setOwned(RubyValue owned);
    RubyValue ext_isNull() const;
    RubyValue ext_toString() const;
    RubyValue ext_destroy();

    static RubyValue objectBaseClass() { return mObjectBaseClass; }
    static void initClass();

private:

    void mark();

    bool mIsOwned = false;
    QPointer<QObject> mObject;
    QJSValue mJSValue;

    static RubyValue mObjectBaseClass;
};

} // namespace Ext
} // namespace RubyQml
