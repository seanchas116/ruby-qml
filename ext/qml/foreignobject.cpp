#include "foreignobject.h"
#include "foreignmetaobject.h"
#include "foreignclass.h"

namespace RubyQml {

ForeignObject::ForeignObject(const SP<ForeignClass> &klass, QObject *parent) :
    QObject(parent),
    mForeignClass(klass)
{
}

const QMetaObject *ForeignObject::metaObject() const
{
    return mForeignClass->metaObject().get();
}

int ForeignObject::qt_metacall(QMetaObject::Call call, int index, void **argv)
{
    return mForeignClass->metaObject()->dynamicMetaCall(this, call, index, argv);
}

} // namespace RubyQml
