#pragma once

#include "extbase.h"

namespace RubyQml {

namespace Ext {

class GCMarker : public ExtBase<GCMarker>
{
    friend class ExtBase<GCMarker>;
public:
    GCMarker();

    void setMarkFunction(const std::function<void()> &func);

private:
    void mark();
    static ClassBuilder buildClass();

    std::function<void ()> mMarkFunc;
};

} // namespace Ext

} // namespace RubyQml
