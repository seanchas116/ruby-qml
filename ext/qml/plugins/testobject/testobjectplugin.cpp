#include "testobjectplugin.h"
#include "testobject.h"
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

} // namespace RubyQml
