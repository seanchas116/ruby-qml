#pragma once
#include <QObject>
#include "rubyvalue.h"
#include "markable.h"

namespace RubyQml {

class ObjectData : public QObject, public Markable
{
public:
    ObjectData(QObject *target);
    RubyValue rubyObject;
    bool owned = false;

    void gc_mark() override;

    static ObjectData *getOrCreate(QObject *target);
};

} // namespace RubyQml
