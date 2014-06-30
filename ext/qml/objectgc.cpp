#include "objectgc.h"
#include "markable.h"
#include "objectdata.h"
#include <QObject>
#include <QDebug>
#include <QBuffer>

namespace RubyQml {

void ObjectGC::addObject(QObject *obj)
{
    mObjects << obj;
    connect(obj, &QObject::destroyed, this, [this](QObject *obj) {
        mObjects.remove(obj);
    });
}

void ObjectGC::mark(QObject *obj, bool markOwned)
{
    auto objData = dynamic_cast<ObjectData *>(obj);
    if (!markOwned) {
        if (objData && objData->owned) {
            return;
        }
    }

    auto markable = dynamic_cast<Markable *>(obj);
    if (markable) {
        debug() << "\u270F\uFE0F  marking object:" << obj << "parent:" << obj->parent();
        markable->gc_mark();
    }
    for (auto child : obj->children()) {
        mark(child, markOwned);
    }
}

void ObjectGC::markOwnedObject(QObject *obj)
{
    mark(obj, true);
}

void ObjectGC::markNonOwnedObjects()
{
    for (auto obj : mObjects) {
        mark(obj, false);
    }
}

QDebug ObjectGC::debug()
{
    static auto emptyBuffer = [] {
        auto buf = new QBuffer();
        buf->open(QIODevice::WriteOnly);
        return buf;
    }();
    if (mDebugMessageEnabled) {
        return qDebug();
    } else {
        return QDebug(emptyBuffer);
    }
}

ObjectGC *ObjectGC::instance()
{
    static auto gc = new ObjectGC();
    return gc;
}

} // namespace RubyQml
