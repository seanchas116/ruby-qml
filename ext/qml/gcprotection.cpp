#include "gcprotection.h"

namespace RubyQml {

void GCProtection::mark() const
{
    for (VALUE val : mValues) {
        rb_gc_mark(val);
    }
}

void GCProtection::add(VALUE x)
{
    mValues << x;
}

void GCProtection::remove(VALUE x)
{
    mValues.remove(x);
}

GCProtection *GCProtection::instance()
{
    if (!mInstance) {
        mRubyInstance = GCProtection::newAsRuby();
        mInstance = GCProtection::fromRuby(mRubyInstance);
        rb_gc_register_address(mRubyInstance);
    }
    return mInstance;
}

GCProtection *GCProtection::mInstance = nullptr;
VALUE GCProtection::mRubyInstance = Qnil;

} // namespace RubyQml
