#include "enginewrapper.h"
#include <QQmlEngine>

namespace RubyQml {

EngineWrapper::EngineWrapper(QQmlEngine *engine) :
    mEngine(engine)
{
}

} // namespace RubyQml
