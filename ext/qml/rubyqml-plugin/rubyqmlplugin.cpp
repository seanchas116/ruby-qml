#include "rubyqmlplugin.h"
#include "listmodel.h"
#include <QQmlEngine>
#include <QQmlContext>

namespace RubyQml {

RubyQmlPlugin::RubyQmlPlugin(QObject *parent) :
    QObject(parent)
{
}

QJSValue RubyQmlPlugin::createListModel(const QJSValue &rubyModelAccess)
{
    QObject *access = rubyModelAccess.toQObject();
    Q_ASSERT(access);
    QQmlEngine *engine = QQmlEngine::contextForObject(access)->engine();

    ListModel *listModel = new ListModel(rubyModelAccess);
    QQmlEngine::setObjectOwnership(listModel, QQmlEngine::JavaScriptOwnership);

    return engine->newQObject(listModel);
}

}
