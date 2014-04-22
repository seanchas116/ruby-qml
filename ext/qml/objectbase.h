#pragma once
#include "rubyclassbase.h"
#include <QtCore/QPointer>

namespace RubyQml {

class ObjectBase : public RubyClassBase<ObjectBase>
{
    friend class RubyClassBase<ObjectBase>;

public:
    ObjectBase();
    ~ObjectBase();

    QObject *qObject();
    void setQObject(QObject *obj, bool hasOwnership = false);
    void setOwnership(bool ownership);

    VALUE hasOwnership() const;
    VALUE withOwnership() const;
    VALUE isNull() const;
    VALUE toString() const;

private:

    void mark() {}
    static Definition createDefinition();

    bool mHasOwnership = false;
    QPointer<QObject> mObject;
};

} // namespace RubyQml

