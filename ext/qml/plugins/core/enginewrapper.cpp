#include "enginewrapper.h"
#include <QQmlEngine>

namespace RubyQml {

EngineWrapper::EngineWrapper(QQmlEngine *engine) :
    mEngine(engine)
{
}

QQmlContext *EngineWrapper::rootContext()
{
    return mEngine->rootContext();
}

} // namespace RubyQml
