#include "objectgc.h"
#include "markable.h"
#include "objectdata.h"
#include <QObject>

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
    if (dynamic_cast<ObjectData *>(obj)) {
        return;
    }
    if (!markOwned) {
        auto data = ObjectData::getOrCreate(obj);
        if (data->owned) {
            return;
        }
    }
    auto markable = dynamic_cast<Markable *>(obj);
    if (markable) {
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

static ObjectGC *instance_ = nullptr;

ObjectGC *ObjectGC::instance()
{
    if (!instance_) {
        instance_ = new ObjectGC();
    }
    return instance_;
}

void ObjectGC::cleanUp()
{
    if (instance_) {
        delete instance_;
    }
}

} // namespace RubyQml
