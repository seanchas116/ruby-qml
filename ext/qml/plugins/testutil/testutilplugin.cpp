#include "testutilplugin.h"
#include "testobject.h"
#include "testobjectsubclass.h"
#include "ownershiptest.h"
#include <QtQml>

namespace RubyQml {

TestUtilPlugin::TestUtilPlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TestObject *>();
    qRegisterMetaType<TestObjectSubclass *>();
    qRegisterMetaType<OwnershipTest *>();
}

TestObject *TestUtilPlugin::createTestObject()
{
    return new TestObject();
}

TestObjectSubclass *TestUtilPlugin::createTestObjectSubclass()
{
    return new TestObjectSubclass();
}

OwnershipTest *TestUtilPlugin::createOwnershipTest()
{
    return new OwnershipTest();
}

} // namespace RubyQml
