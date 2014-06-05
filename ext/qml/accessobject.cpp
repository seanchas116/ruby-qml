#include "accessobject.h"
#include "accessclass.h"
#include "util.h"
#include <QSet>

namespace RubyQml {

AccessObject::AccessObject(const SP<ForeignClass> &klass, VALUE value) :
    ForeignClass::Object(klass),
    mValue(value)
{
    globalMarkValues() << value;
}

AccessObject::~AccessObject()
{
    globalMarkValues().remove(mValue);
}

} // namespace RubyQml
