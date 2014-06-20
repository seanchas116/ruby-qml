#include "objectlifechecker.h"
#include <QQmlEngine>

namespace RubyQml {

ObjectLifeChecker::ObjectLifeChecker(QObject *target) :
    mTarget(target)
{
}

bool ObjectLifeChecker::isOwnedByQml() const
{
    if (!mTarget) return false;
    return QQmlEngine::objectOwnership(mTarget) == QQmlEngine::JavaScriptOwnership;
}

} // namespace RubyQml
