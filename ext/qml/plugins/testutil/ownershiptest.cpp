#include "ownershiptest.h"

namespace RubyQml {

OwnershipTest::OwnershipTest(QObject *parent) :
    QObject(parent)
{
    mSubObject = new QObject(this);
    connect(mSubObject, &QObject::destroyed, this, &OwnershipTest::subObjectDeleted);
    mPropertyObject = new QObject();
    connect(mPropertyObject, &QObject::destroyed, this, &OwnershipTest::subObjectDeleted);
}

OwnershipTest::~OwnershipTest()
{
    delete mPropertyObject;
}

QObject *OwnershipTest::createObject()
{
    auto obj = new QObject();
    connect(obj, &QObject::destroyed, this, &OwnershipTest::createdObjectDeleted);
    return obj;
}

} // namespace RubyQml
