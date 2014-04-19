#include "objectbase.h"
#include <QtCore/QObject>
#include <QtCore/QHash>

namespace RubyQml {

ObjectBase::ObjectBase()
{
}

ObjectBase::ObjectBase(QObject *object, bool hasOwnership) :
    mHasOwnership(hasOwnership),
    mObject(object)
{
    if (!mObject) {
        mHasOwnership = false;
    }
    if (mHasOwnership) {
        // multiple ObjectBase may refer the same QObject
        mObjRefCount[mObject] = mObjRefCount.value(mObject, 0) + 1;
    }
}

ObjectBase::~ObjectBase()
{
    if (mHasOwnership) {
        mObjRefCount[mObject] -= 1;
        if (mObjRefCount[mObject] == 0) {
            mObjRefCount.remove(mObject);
            if (!mObject->parent()) {
                mObject->deleteLater();
            }
        }
    }
}

ObjectBase::Definition ObjectBase::createDefinition()
{
    return Definition("QML", "ObjectBase");
}

} // namespace RubyQml
