#pragma once

#include "rubyclassbase.h"

namespace RubyQml {

class GCMarker : public RubyClassBase<GCMarker>
{
    friend class RubyClassBase<GCMarker>;
public:
    GCMarker();

    void setMarkFunction(const std::function<void()> &func);

private:
    void mark();
    static ClassBuilder buildClass();

    std::function<void ()> mMarkFunc;
};

} // namespace RubyQml
