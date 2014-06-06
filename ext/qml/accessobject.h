#pragma once
#include <ruby.h>
#include "foreignobject.h"

namespace RubyQml {


class AccessObject : public ForeignObject
{
public:
    AccessObject(const SP<ForeignClass> &klass, VALUE value);
    ~AccessObject();

    VALUE value() { return mValue; }

private:
    VALUE mValue;
};

} // namespace RubyQml

