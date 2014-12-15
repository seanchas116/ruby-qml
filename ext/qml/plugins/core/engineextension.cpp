#include "engineextension.h"
#include "imageprovider.h"
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

void EngineExtension::addImageProvider(const QString &id, ImageProvider *provider)
{
    mEngine->addImageProvider(id, provider);
}

void EngineExtension::addImportPath(const QString &path)
{
    mEngine->addImportPath(path);
}

void EngineExtension::collectGarbage()
{
    mEngine->collectGarbage();
}

} // namespace RubyQml
