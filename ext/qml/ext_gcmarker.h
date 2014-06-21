#pragma once

#include "rubyvalue.h"
#include <functional>

namespace RubyQml {

namespace Ext {

class GCMarker
{
public:
    GCMarker(RubyValue self);
    static RubyValue fromMarkFunction(const std::function<void ()> &func);

    void setMarkFunction(const std::function<void()> &func);
    void gc_mark();

    static void defineClass();

private:
    std::function<void ()> mMarkFunc;
};

} // namespace Ext

} // namespace RubyQml
