#include "rubyqmlplugin.h"

RubyQmlPlugin::RubyQmlPlugin(QObject *parent) :
    QObject(parent)
{
}

QJSValue RubyQmlPlugin::createListModel(const QJSValue &rubyListModel)
{
    // TODO
    Q_UNUSED(rubyListModel);
    return QJSValue();
}
