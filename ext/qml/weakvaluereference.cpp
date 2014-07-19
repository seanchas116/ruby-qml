#include "weakvaluereference.h"
#include "rubyclass.h"

namespace RubyQml {

WeakValueReference::WeakValueReference(RubyValue value) :
    mHasValue(true),
    mValue(value)
{
    static auto objspace = RubyModule::fromPath("ObjectSpace");
    protect([&] {
        auto proc = rb_proc_new((VALUE (*)(...))&finalize, VALUE(this));
        VALUE args[] = { value };
        rb_funcall_with_block(objspace, RUBYQML_INTERN("define_finalizer"), 1, args, proc);
    });
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
