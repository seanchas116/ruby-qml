#include "componentwrapper.h"
#include "qmlexception.h"
#include <QQmlComponent>
#include <QDebug>

namespace RubyQml {

ComponentWrapper::ComponentWrapper(QQmlComponent *component) :
    mComponent(component)
{
}

void ComponentWrapper::loadString(const QString &data, const QString &path)
{
    mComponent->setData(data.toUtf8(), QUrl::fromLocalFile(path));
    throwIfError();
}

void ComponentWrapper::loadFile(const QString &filePath)
{
    mComponent->loadUrl(QUrl::fromLocalFile(filePath));
    throwIfError();
}

QObject *ComponentWrapper::create()
{
    auto created = mComponent->create();
    throwIfError();
    return created;
}

void ComponentWrapper::throwIfError()
{
    if (mComponent->status() == QQmlComponent::Error) {
        throw QmlException(mComponent->errorString());
    }
}

} // namespace RubyQml
