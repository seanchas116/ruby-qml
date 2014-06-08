#include "foreignobject.h"
#include "foreignmetaobject.h"

namespace RubyQml {

ForeignObject::ForeignObject(const SP<ForeignMetaObject> &metaobj, QObject *parent) :
    QObject(parent),
    mMetaObject(metaobj)
{
}

const QMetaObject *ForeignObject::metaObject() const
{
    return mMetaObject.get();
}

int ForeignObject::qt_metacall(QMetaObject::Call call, int index, void **argv)
{
    return mMetaObject->dynamicMetaCall(this, call, index, argv);
}

} // namespace RubyQml
