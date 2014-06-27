#include "objectdata.h"

namespace RubyQml {

ObjectData::ObjectData(QObject *target) :
    QObject(target)
{
}

ObjectData *ObjectData::getOrCreate(QObject *target)
{
    for (auto child : target->children()) {
        auto data = dynamic_cast<ObjectData *>(child);
        if (data) {
            return data;
        }
    }
    return new ObjectData(target);
}

void ObjectData::gc_mark()
{
    rb_gc_mark(wrapper);
}

} // namespace RubyQml
