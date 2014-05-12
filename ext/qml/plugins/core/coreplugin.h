#pragma once

#include <QObject>

namespace RubyQml {

class CorePlugin : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ruby-qml.RubyQml.Core")

public:
    CorePlugin(QObject *parent = 0);
};


} // namespace RubyQml
