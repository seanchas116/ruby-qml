#pragma once
#include "rubyclassbase.h"

namespace RubyQml {

class ObjectBase : public RubyClassBase<ObjectBase>
{
public:
    ObjectBase(QObject *object);
    ~ObjectBase();

    QObject *qObject() { return mObject; }

private:
    QObject *mObject = nullptr;
};

} // namespace RubyQml

