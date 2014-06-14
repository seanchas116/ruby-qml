#pragma once
#include "valuereference.h"
#include "foreignobject.h"

namespace RubyQml {

class AccessObject : public ForeignObject
{
public:
    AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue value);

    RubyValue value() { return mRef.value(); }

private:
    ValueReference mRef;
};

} // namespace RubyQml

