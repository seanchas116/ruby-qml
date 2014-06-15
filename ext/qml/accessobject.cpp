#include "accessobject.h"
#include "util.h"
#include <QSet>

namespace RubyQml {

AccessObject::AccessObject(const SP<ForeignMetaObject> &metaobj, RubyValue value) :
    ForeignObject(metaobj),
    mValue(value)
{
}

void AccessObject::gc_mark()
{
    rb_gc_mark(mValue);
}

} // namespace RubyQml
