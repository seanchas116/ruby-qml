#include "engineextension.h"
#include <QQmlEngine>

namespace RubyQml {

EngineExtension::EngineExtension(QQmlEngine *engine) :
    mEngine(engine)
{
}

QQmlContext *EngineExtension::rootContext()
{
    return mEngine->rootContext();
}

} // namespace RubyQml
