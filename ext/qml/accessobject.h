#pragma once
#include "weakvaluereference.h"
#include "foreignobject.h"
#include "markable.h"

namespace RubyQml {

class AccessObject : public ForeignObject, public Markable
{
public:
    AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue wrappedValue);
    ~AccessObject();

    RubyValue wrappedValue() { return mWrapped.value(); }
    void gc_mark() override;

private:
    WeakValueReference mWrapped;
};

} // namespace RubyQml

