#pragma once
#include "rubyclassbase.h"
#include <QtCore/QPointer>

namespace RubyQml {

class ObjectPointer : public RubyClassBase<ObjectPointer>
{
    friend class RubyClassBase<ObjectPointer>;

public:
    ObjectPointer();
    ~ObjectPointer();

    QObject *qObject();
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

} // namespace RubyQml

