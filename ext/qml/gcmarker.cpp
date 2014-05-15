#include "gcmarker.h"
#include <QtCore/QSet>

namespace RubyQml {

GCMarker::GCMarker()
{
}

void GCMarker::setMarkFunction(const std::function<void ()> &func)
{
    mMarkFunc = func;
}

void GCMarker::mark()
{
    if (mMarkFunc)
        mMarkFunc();
}

GCMarker::ClassBuilder GCMarker::buildClass()
{
    ClassBuilder builder("QML", "GCProtection");
    return builder;
}

} // namespace RubyQml
