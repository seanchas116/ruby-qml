#include "objectdata.h"
#include "extension.h"
#include "gcprotection.h"

namespace RubyQml {

ObjectData::ObjectData(VALUE rubyObject) :
    mRubyObject(rubyObject)
{
    GCProtection::add(mRubyObject);
}

ObjectData::~ObjectData()
{
    GCProtection::remove(mRubyObject);
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
