#pragma once
#include "rubyvalue.h"

namespace RubyQml {

struct RubyClasses
{
    RubyClasses();

    RubyValue access;
    RubyValue qtObjectBase;

    static void initialize();
};

const RubyClasses &rubyClasses();

} // namespace RubyQml
