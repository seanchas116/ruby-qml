#include "gcprotection.h"
#include <QtCore/QSet>

namespace RubyQml {

GCProtection::GCProtection()
{
}

void GCProtection::add(VALUE x)
{
    mValues << x;
}

void GCProtection::remove(VALUE x)
{
    mValues.remove(x);
}

void GCProtection::mark()
{
    for (auto x : mValues) {
        rb_gc_mark(x);
    }
}

GCProtection::ClassBuilder GCProtection::buildClass()
{
    ClassBuilder builder("QML", "GCProtection");
    return builder;
}

} // namespace RubyQml
