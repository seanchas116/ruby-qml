#pragma once
#include "rubyvalue.h"
#include "common.h"

namespace RubyQml {

class ValueReference
{
public:
    ValueReference() = default;
    ValueReference(RubyValue value);

    RubyValue value() const;

    static void markAllReferences();

private:
    struct Owner;
    SP<Owner> mOwner;
};

} // namespace RubyQml
