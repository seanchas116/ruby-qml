#pragma once
#include <QSet>
#include <QObject>

namespace RubyQml {

class ObjectGC : public QObject {
public:

    void addObject(QObject *obj);
    void markOwnedObject(QObject *obj);
    void markNonOwnedObjects();

    void setDebugMessageEnabled(bool enabled) { mDebugMessageEnabled = enabled; }
    bool debugMessageEnabled() { return mDebugMessageEnabled; }
    QDebug debug();

    static ObjectGC *instance();
    static void cleanUp();

private:
    ObjectGC() {}
    void mark(QObject *obj, bool markOwned);

    bool mDebugMessageEnabled = false;
    QSet<QObject *> mObjects;
};

} // namespace RubyQml
