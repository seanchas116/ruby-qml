#include "accessobject.h"
#include "accessclass.h"
#include "util.h"
#include <QSet>
#include <QQmlEngine>

namespace RubyQml {

AccessObject::AccessObject(const SP<ForeignClass> &klass, VALUE value) :
    ForeignObject(klass),
    mValue(value)
{
    globalMarkValues() << value;
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
}

AccessObject::~AccessObject()
{
    globalMarkValues().remove(mValue);
}

} // namespace RubyQml
