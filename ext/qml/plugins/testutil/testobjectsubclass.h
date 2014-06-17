#pragma once

#include "testobject.h"

namespace RubyQml {

class TestObjectSubclass : public TestObject
{
    Q_OBJECT
public:
    explicit TestObjectSubclass(QObject *parent = 0);

signals:

public slots:

};

} // namespace RubyQml
