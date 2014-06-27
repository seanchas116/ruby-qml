#include "objectgc.h"
#include "markable.h"
#include "objectdata.h"
#include <QObject>
#include <QDebug>

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
        qDebug() << "marking:" << obj << "parent:" << obj->parent();
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
