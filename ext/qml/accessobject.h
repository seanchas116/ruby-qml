#pragma once
#include <ruby.h>
#include "foreignclass_object.h"

namespace RubyQml {


class AccessObject : public ForeignClass::Object
{
public:
    AccessObject(const SP<ForeignClass> &klass, VALUE value);
    ~AccessObject();

    VALUE value() { return mValue; }

private:
    VALUE mValue;
};

} // namespace RubyQml

