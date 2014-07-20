#pragma once
#include "rubyvalue.h"

namespace RubyQml {

class WeakValueReference
{
public:
    WeakValueReference(RubyValue value);

    bool hasValue() const;
    RubyValue value() const;

    struct Data;

private:
    std::shared_ptr<Data> d;
};

} // namespace RubyQml
