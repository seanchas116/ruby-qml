#include "weakvaluereference.h"
#include "ext_anywrapper.h"
#include "rubyclass.h"

Q_DECLARE_METATYPE(std::shared_ptr<RubyQml::WeakValueReference::Data>)

namespace RubyQml {

struct WeakValueReference::Data
{
    bool mHasValue;
    RubyValue mValue;

    static VALUE finalize(VALUE args, VALUE data) {
        Q_UNUSED(args);
        auto variant = wrapperRubyClass<Ext::AnyWrapper>().unwrap(data)->value();
        auto sp = variant.value<std::shared_ptr<Data>>();
        sp->invalidiate();
        return Qnil;
    }

    void invalidiate()
    {
        mHasValue = false;
        mValue = Qnil;
    }
};

WeakValueReference::WeakValueReference(RubyValue value) :
    d(std::make_shared<Data>())
{
    d->mHasValue = true;
    d->mValue = value;
    static auto objspace = RubyModule::fromPath("ObjectSpace");
    protect([&] {
        auto proc = rb_proc_new((VALUE (*)(...))&Data::finalize, Ext::AnyWrapper::create(QVariant::fromValue(d)));
        VALUE args[] = { value };
        rb_funcall_with_block(objspace, RUBYQML_INTERN("define_finalizer"), 1, args, proc);
    });
}

bool WeakValueReference::hasValue() const
{
    return d->mHasValue;
}

RubyValue WeakValueReference::value() const
{
    return d->mValue;
}

} // namespace RubyQml
