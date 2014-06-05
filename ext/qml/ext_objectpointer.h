#pragma once
#include "extbase.h"
#include <QtCore/QPointer>

namespace RubyQml {
namespace Ext {

class ObjectPointer : public ExtBase<ObjectPointer>
{
    friend class ExtBase<ObjectPointer>;

public:
    ObjectPointer();
    ~ObjectPointer();

    QObject *qObject() { return mObject; }
    QObject *fetchQObject();
    void setQObject(QObject *obj, bool hasOwnership = false);
    void setOwnership(bool ownership);

    VALUE hasOwnership() const;
    VALUE withOwnership() const;
    VALUE isNull() const;
    VALUE toString() const;

    static VALUE objectBaseClass() { return mObjectBaseClass; }

private:

    void mark() {}
    static ClassBuilder buildClass();

    bool mHasOwnership = false;
    QPointer<QObject> mObject;

    static VALUE mObjectBaseClass;
};

} // namespace Ext
} // namespace RubyQml
