#include "ext_gcmarker.h"
#include <QtCore/QSet>

namespace RubyQml {
namespace Ext {

GCMarker::GCMarker()
{
}

void GCMarker::setMarkFunction(const std::function<void ()> &func)
{
    mMarkFunc = func;
}

RubyValue GCMarker::fromMarkFunction(const std::function<void ()> &func)
{
    auto marker = newAsRuby();
    getPointer(marker)->setMarkFunction(func);
    return marker;
}

void GCMarker::mark()
{
    if (mMarkFunc)
        mMarkFunc();
}

void GCMarker::initClass()
{
    ClassBuilder builder("QML", "GCProtection");
    Q_UNUSED(builder);
}

} // namespace Ext
} // namespace RubyQml
