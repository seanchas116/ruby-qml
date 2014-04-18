#pragma once
#include <QtCore/QSet>
#include "rubyclassbase.h"

namespace RubyQml {

class GCProtection : public RubyClassBase<GCProtection>
{
public:

    void mark() const;

    void add(VALUE x);
    void remove(VALUE x);

    static GCProtection *instance();

private:

    QSet<VALUE> mValues;
    static GCProtection *mInstance;
    static VALUE mRubyInstance;
};

} // namespace RubyQml
