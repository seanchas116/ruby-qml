#pragma once
#include "extbase.h"
#include <QtCore/QPointer>

namespace RubyQml {
namespace Ext {

class QtObjectPointer : public ExtBase<QtObjectPointer>
{
    friend class ExtBase<QtObjectPointer>;

public:
    QtObjectPointer();
    ~QtObjectPointer();

    static RubyValue fromQObject(QObject *obj, bool hasOwnership);

    QObject *qObject() { return mObject; }
    QObject *fetchQObject();
    void setQObject(QObject *obj, bool hasOwnership);
    void setOwnership(bool ownership);

    RubyValue hasOwnership() const;
    RubyValue withOwnership() const;
    RubyValue isNull() const;
    RubyValue toString() const;
    RubyValue destroy();

    static RubyValue objectBaseClass() { return mObjectBaseClass; }
    static void initClass();

private:

    void mark() {}

    bool mHasOwnership = false;
    QPointer<QObject> mObject;

    static RubyValue mObjectBaseClass;
};

} // namespace Ext
} // namespace RubyQml
