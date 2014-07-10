#pragma once
#include "weakvaluereference.h"
#include "foreignobject.h"
#include "markable.h"

namespace RubyQml {

class AccessWrapper : public ForeignObject, public Markable
{
public:
    AccessWrapper(const SP<ForeignMetaObject> &metaobj, RubyValue wrappedValue);
    ~AccessWrapper();

    RubyValue wrappedValue() { return mWrapped.value(); }
    void gc_mark() override;

private:
    WeakValueReference mWrapped;
};

} // namespace RubyQml

