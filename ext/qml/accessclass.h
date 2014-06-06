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

    QVariant callMethod(ForeignObject *obj, size_t id, const QVariantList &args) override;
    void setProperty(ForeignObject *obj, size_t id, const QVariant &value) override;
    QVariant getProperty(ForeignObject *obj, size_t id) override;
};

} // namespace RubyQml
