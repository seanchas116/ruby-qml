#pragma once
#include "rubyclass.h"

namespace RubyQml {

struct RubyClasses
{
    RubyClasses();

    RubyModule access;
    RubyModule wrapper;

    static void initialize();
};

const RubyClasses &rubyClasses();

} // namespace RubyQml
