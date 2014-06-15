#pragma once

namespace RubyQml {

class Markable
{
public:
    virtual ~Markable() {}
    virtual void gc_mark() = 0;
};

} // namespace RubyQml
