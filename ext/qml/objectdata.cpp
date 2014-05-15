#include "objectdata.h"
#include "util.h"
#include <QtCore/QSet>
#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace RubyQml {

ObjectData::ObjectData(VALUE rubyObject) :
    mRubyObject(rubyObject)
{
    globalMarkValues() << mRubyObject;
}

ObjectData::~ObjectData()
{
    globalMarkValues().remove(mRubyObject);
}

std::shared_ptr<ObjectData> ObjectData::get(QObject *obj)
{
    return obj->property("rubyqml_data").value<std::shared_ptr<ObjectData>>();
}

void ObjectData::set(QObject *obj, const std::shared_ptr<ObjectData> &data)
{
    obj->setProperty("rubyqml_data", QVariant::fromValue(data));
}

} // namespace RubyQml
