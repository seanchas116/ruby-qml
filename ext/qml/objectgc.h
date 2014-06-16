#pragma once
#include <QSet>
#include <QObject>

namespace RubyQml {

class ObjectGC : public QObject {
public:

    void addObject(QObject *obj);
    void markOwnedObject(QObject *obj);
    void markNonOwnedObjects();
    static ObjectGC *instance();
    static void cleanUp();

private:
    ObjectGC() {}
    void mark(QObject *obj, bool markOwned);

    QSet<QObject *> mRootObjects;
};

} // namespace RubyQml
