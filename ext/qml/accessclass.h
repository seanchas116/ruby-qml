#pragma once
#include "foreignclass.h"
#include <ruby.h>

namespace RubyQml {

class AccessObject;

class AccessClass : public ForeignClass
{
public:
    AccessClass(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos);

    AccessObject *newAccessObject(VALUE obj);

    QVariant callMethod(Object *obj, size_t id, const QVariantList &args) override;
    void setProperty(Object *obj, size_t id, const QVariant &value) override;
    QVariant getProperty(Object *obj, size_t id) override;
};

} // namespace RubyQml
