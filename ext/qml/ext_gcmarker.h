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
    static void initClass();
    static RubyValue fromMarkFunction(const std::function<void ()> &func);

private:
    void mark();

    std::function<void ()> mMarkFunc;
};

} // namespace Ext

} // namespace RubyQml
