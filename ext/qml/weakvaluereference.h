#pragma once
#include "rubyvalue.h"
#include <memory>

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
