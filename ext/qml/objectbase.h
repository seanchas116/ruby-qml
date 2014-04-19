#pragma once
#include "rubyclassbase.h"

namespace RubyQml {

class ObjectBase : public RubyClassBase<ObjectBase>
{
    friend class RubyClassBase<ObjectBase>;

public:
    ObjectBase();
    ObjectBase(QObject *object, bool hasOwnership = true);
    ~ObjectBase();

    QObject *qObject() { return mObject; }

private:

    static Definition createDefinition();

    bool mHasOwnership = false;
    QObject *mObject = nullptr;
    static QHash<QObject *, int> mObjRefCount;
};

} // namespace RubyQml

