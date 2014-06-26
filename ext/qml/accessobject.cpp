#include "accessobject.h"
#include "util.h"
#include <QSet>

namespace RubyQml {

AccessObject::AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue value) :
    ForeignObject(metaobj),
    mWrapped(value)
{
    if (!value.isKindOf(RubyValue::fromPath("QML::Access"))) {
        std::logic_error("wrapping non QML::Access object");
    }
    value.send("access_objects").send("push", RubyValue::fromQObject(this, false));
}

AccessObject::~AccessObject()
{
    if (mWrapped.hasValue()) {
        mWrapped.value().send("access_objects").send("delete", RubyValue::fromQObject(this, false));
    }
}

void AccessObject::gc_mark()
{
    rb_gc_mark(mWrapped.value());
}

} // namespace RubyQml
