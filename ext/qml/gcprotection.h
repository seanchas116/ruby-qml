#pragma once

#include "rubyclassbase.h"
#include <QtCore/QSet>

namespace RubyQml {

class GCProtection : public RubyClassBase<GCProtection>
{
    friend class RubyClassBase<GCProtection>;
public:
    GCProtection();

    void add(VALUE x);
    void remove(VALUE x);

private:
    void mark();
    static ClassBuilder buildClass();

    QSet<VALUE> mValues;
};

} // namespace RubyQml
