#include "accessobject.h"
#include "util.h"
#include "rubyclass.h"
#include <QSet>

namespace RubyQml {

AccessWrapper::AccessWrapper(const SP<ForeignMetaObject> &metaobj, RubyValue value) :
    ForeignObject(metaobj),
    mWrapped(value)
{
    if (!value.isKindOf(RubyModule::fromPath("QML::Access"))) {
        std::logic_error("wrapping non QML::Access object");
    }
    value.send("access_wrappers").send("push", RubyValue::fromQObject(this, false));
}

AccessWrapper::~AccessWrapper()
{
    if (mWrapped.hasValue()) {
        mWrapped.value().send("access_wrappers").send("delete", RubyValue::fromQObject(this, false));
    }
}

void AccessWrapper::gc_mark()
{
    rb_gc_mark(mWrapped.value());
}

} // namespace RubyQml
