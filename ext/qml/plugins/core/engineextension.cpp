#include "engineextension.h"
#include "imageprovider.h"
#include <QQmlEngine>
#include <QStringList>

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

QVariantList EngineExtension::importPaths() const
{
    QStringList pathList = mEngine->importPathList();
    QVariantList varList;
    for(auto path : pathList){
        varList << path;
    }
    return varList;
}

void EngineExtension::collectGarbage()
{
    mEngine->collectGarbage();
}

} // namespace RubyQml
