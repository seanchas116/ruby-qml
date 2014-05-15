#pragma once

#include <QtCore/QObject>

namespace RubyQml {

class GCProtection;

class Extension : public QObject
{
    //Q_OBJECT
public:
    explicit Extension(QObject *parent = 0);

    GCProtection *protection() { return mProtection; }

    static Extension *instance();

private:
    GCProtection *mProtection = nullptr;
};

} // namespace RubyQml
