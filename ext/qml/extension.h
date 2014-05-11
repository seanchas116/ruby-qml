#pragma once

#include <QtCore/QObject>

namespace RubyQml {

class Extension : public QObject
{
    //Q_OBJECT
public:
    explicit Extension(QObject *parent = 0);

    static Extension *instance();
};

} // namespace RubyQml
