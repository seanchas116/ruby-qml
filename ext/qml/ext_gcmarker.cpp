#include "ext_gcmarker.h"
#include "rubyclass.h"
#include <QtCore/QSet>

namespace RubyQml {
namespace Ext {

GCMarker::GCMarker(RubyValue self)
{
    Q_UNUSED(self)
}

RubyValue GCMarker::fromMarkFunction(const std::function<void ()> &func)
{
    auto klass = wrapperRubyClass<GCMarker>();
    auto marker = klass.newInstance();
    klass.unwrap(marker)->setMarkFunction(func);
    return marker;
}

void GCMarker::setMarkFunction(const std::function<void ()> &func)
{
    mMarkFunc = func;
}

void GCMarker::gc_mark()
{
    if (mMarkFunc)
        mMarkFunc();
}

void GCMarker::defineClass()
{
    WrapperRubyClass<GCMarker> klass("QML", "GCProtection");
    Q_UNUSED(klass);
}

} // namespace Ext
} // namespace RubyQml
