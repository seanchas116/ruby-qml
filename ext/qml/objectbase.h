#pragma once
#include "rubyclassbase.h"

namespace RubyQml {

class ObjectBase : public RubyClassBase<ObjectBase>
{
    friend class RubyClassBase<ObjectBase>;

public:
    ObjectBase();
    ~ObjectBase();

    QObject *qObject() { return mObject; }
    void setQObject(QObject *obj, bool hasOwnership = true);

private:

    static Definition createDefinition();

    bool mHasOwnership = false;
    QObject *mObject = nullptr;
    static QHash<QObject *, int> mObjRefCount;
};

} // namespace RubyQml

