#pragma once
#include "rubyvalue.h"

namespace RubyQml {

class WeakValueReference
{
public:
    WeakValueReference(RubyValue value);

    bool hasValue() const { return mHasValue; }
    RubyValue value() const { return mValue; }
private:
    static VALUE finalize(VALUE arg, VALUE data);
    bool mHasValue;
    RubyValue mValue;
};

} // namespace RubyQml
