#include "testobjectplugin.h"
#include "testobject.h"
#include "testobjectsubclass.h"
#include <QtQml>

namespace RubyQml {

TestObjectPlugin::TestObjectPlugin(QObject *parent) :
    QObject(parent)
{
}

QObject *TestObjectPlugin::createTestObject()
{
    return new TestObject();
}

QObject *TestObjectPlugin::createTestObjectSubclass()
{
    return new TestObjectSubclass();
}

} // namespace RubyQml
