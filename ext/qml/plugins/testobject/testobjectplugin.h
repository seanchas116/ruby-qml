#pragma once
#include <QObject>
#include "testobject.h"

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
    QObject *createTestObject();

private:

};

} // namespace RubyQml
