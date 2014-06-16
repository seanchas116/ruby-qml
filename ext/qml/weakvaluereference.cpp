#include "weakvaluereference.h"

namespace RubyQml {

WeakValueReference::WeakValueReference(RubyValue value) :
    mHasValue(true),
    mValue(value)
{
    rb_define_finalizer(mValue, rb_proc_new((VALUE (*)(...))&finalize, VALUE(this)));
}

VALUE WeakValueReference::finalize(VALUE arg, VALUE data)
{
    Q_UNUSED(arg);
    auto reference = (WeakValueReference *)data;
    reference->mHasValue = false;
    reference->mValue = Qnil;
    return Qnil;
}

} // namespace RubyQml
