#pragma once
#include <QObject>

namespace RubyQml {

class TestObject;

class TestObjectPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.TestObject")
public:
    explicit TestObjectPlugin(QObject *parent = 0);
signals:

public slots:
    RubyQml::TestObject *createTestObject();
    QObject *createTestObjectSubclass();

private:

};

} // namespace RubyQml
