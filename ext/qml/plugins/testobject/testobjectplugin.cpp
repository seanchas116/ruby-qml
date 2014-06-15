#include "testobjectplugin.h"
#include "testobject.h"
#include "testobjectsubclass.h"
#include "ownershiptest.h"
#include <QtQml>

namespace RubyQml {

TestObjectPlugin::TestObjectPlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TestObject *>();
    qRegisterMetaType<TestObjectSubclass *>();
    qRegisterMetaType<OwnershipTest *>();
}

TestObject *TestObjectPlugin::createTestObject()
{
    return new TestObject();
}

TestObjectSubclass *TestObjectPlugin::createTestObjectSubclass()
{
    return new TestObjectSubclass();
}

OwnershipTest *TestObjectPlugin::createOwnershipTest()
{
    return new OwnershipTest();
}

} // namespace RubyQml
