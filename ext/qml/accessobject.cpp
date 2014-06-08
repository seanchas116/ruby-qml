#include "accessobject.h"
#include "util.h"
#include <QSet>

namespace RubyQml {

AccessObject::AccessObject(const SP<ForeignMetaObject> &metaobj, VALUE value) :
    ForeignObject(metaobj),
    mValue(value)
{
    globalMarkValues() << value;
}

AccessObject::~AccessObject()
{
    globalMarkValues().remove(mValue);
}

} // namespace RubyQml
