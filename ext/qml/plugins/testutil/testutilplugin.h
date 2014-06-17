#pragma once
#include <QObject>

namespace RubyQml {

class TestObject;
class TestObjectSubclass;
class OwnershipTest;

class TestUtilPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.TestObject")
public:
    explicit TestUtilPlugin(QObject *parent = 0);
signals:

public slots:
    RubyQml::TestObject *createTestObject();
    RubyQml::TestObjectSubclass *createTestObjectSubclass();
    RubyQml::OwnershipTest *createOwnershipTest();

private:

};

} // namespace RubyQml
