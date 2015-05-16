#pragma once

#include <QObject>
#include <QJSValue>

namespace RubyQml {

class RubyQmlPlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml")

public:
    RubyQmlPlugin(QObject *parent = 0);

    Q_INVOKABLE QJSValue createListModel(const QJSValue &rubyModelAccess);
};

}
