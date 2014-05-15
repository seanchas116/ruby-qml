#pragma once

#include "rubyclassbase.h"

namespace RubyQml {

class GCProtection : public RubyClassBase<GCProtection>
{
    friend class RubyClassBase<GCProtection>;
public:
    GCProtection();

    static void add(VALUE x);
    static void remove(VALUE x);

private:
    void mark();
    static ClassBuilder buildClass();

    static QSet<VALUE> mValues;
};

} // namespace RubyQml
