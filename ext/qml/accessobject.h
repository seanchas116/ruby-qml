#pragma once
#include "rubyvalue.h"
#include "foreignobject.h"

namespace RubyQml {

class AccessObject : public ForeignObject
{
public:
    AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue value);
    ~AccessObject();

    RubyValue value() { return mValue; }

private:
    RubyValue mValue;
};

} // namespace RubyQml

