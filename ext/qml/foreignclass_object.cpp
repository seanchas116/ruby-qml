#include "foreignclass_object.h"
#include "foreignclass_metaobject.h"
#include "foreignclass.h"

namespace RubyQml {

ForeignClass::Object::Object(const SP<ForeignClass> &klass, QObject *parent) :
    QObject(parent),
    mForeignClass(klass)
{
}

const QMetaObject *ForeignClass::Object::metaObject() const
{
    return mForeignClass->metaObject().get();
}

int ForeignClass::Object::qt_metacall(QMetaObject::Call call, int index, void **argv)
{
    return mForeignClass->metaObject()->dynamicMetaCall(this, call, index, argv);
}

} // namespace RubyQml
