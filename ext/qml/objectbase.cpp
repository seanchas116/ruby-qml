#include "objectbase.h"
#include <QtCore/QObject>
#include <QtCore/QHash>

namespace RubyQml {

ObjectBase::ObjectBase()
{
}

ObjectBase::~ObjectBase()
{
    setQObject(nullptr);
}

void ObjectBase::setQObject(QObject *obj, bool hasOwnership)
{
    // release previous object
    if (mHasOwnership) {
        mObjRefCount[mObject] -= 1;
        Q_ASSERT(mObjRefCount[mObject] >= 0);
        if (mObjRefCount[mObject] == 0) {
            mObjRefCount.remove(mObject);
            if (!mObject->parent()) {
                mObject->deleteLater();
            }
        }
    }

    mObject = obj;
    mHasOwnership = hasOwnership;
    if (!obj) {
        mHasOwnership = false;
    }

    if (mHasOwnership) {
        mObjRefCount[mObject] = mObjRefCount.value(mObject, 0) + 1;
    }
}

ObjectBase::Definition ObjectBase::createDefinition()
{
    return Definition("QML", "ObjectBase");
}

} // namespace RubyQml
