#include "testutilplugin.h"
#include "testobject.h"
#include "testobjectsubclass.h"
#include "ownershiptest.h"
#include "objectlifechecker.h"
#include "imageprovidertest.h"
#include <QtQml>
#include <QQuickWindow>

Q_DECLARE_METATYPE(const QMetaObject*)

namespace RubyQml {

TestUtilPlugin::TestUtilPlugin(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TestObject *>();
    qRegisterMetaType<TestObject::Enums>();
    qRegisterMetaType<TestObjectSubclass *>();
    qRegisterMetaType<OwnershipTest *>();
    qRegisterMetaType<ObjectLifeChecker *>();
    qRegisterMetaType<ImageProviderTest *>();

    auto metaObjects = { &TestObject::staticMetaObject, &ObjectLifeChecker::staticMetaObject};
    for (auto metaobj : metaObjects) {
        mMetaObjects[metaobj->className()] = QVariant::fromValue(metaobj);
    }
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

ObjectLifeChecker *TestUtilPlugin::createObjectLifeChecker(QObject *target)
{
    return new ObjectLifeChecker(target);
}

ImageProviderTest *TestUtilPlugin::createImageProviderTest()
{
    return new ImageProviderTest();
}

} // namespace RubyQml
