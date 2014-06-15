#pragma once
#include "rubyvalue.h"
#include "foreignobject.h"
#include "markable.h"

namespace RubyQml {

class AccessObject : public ForeignObject, public Markable
{
public:
    AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue value);

    RubyValue value() { return mValue; }
    void gc_mark() override;

private:
    RubyValue mValue;
};

} // namespace RubyQml

