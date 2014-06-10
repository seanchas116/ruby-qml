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

    static VALUE fromQObject(QObject *obj, bool hasOwnership);

    QObject *qObject() { return mObject; }
    QObject *fetchQObject();
    void setQObject(QObject *obj, bool hasOwnership);
    void setOwnership(bool ownership);

    VALUE hasOwnership() const;
    VALUE withOwnership() const;
    VALUE isNull() const;
    VALUE toString() const;

    static VALUE objectBaseClass() { return mObjectBaseClass; }
    static void initClass();

private:

    void mark() {}

    bool mHasOwnership = false;
    QPointer<QObject> mObject;

    static VALUE mObjectBaseClass;
};

} // namespace Ext
} // namespace RubyQml
