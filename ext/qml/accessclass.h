#pragma once
#include "foreignclass.h"
#include "rubyvalue.h"

namespace RubyQml {

class AccessObject;

class AccessClass : public ForeignClass
{
public:
    AccessClass(RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos);

    QVariant callMethod(ForeignObject *obj, size_t id, const QVariantList &args) override;
    void setProperty(ForeignObject *obj, size_t id, const QVariant &value) override;
    QVariant getProperty(ForeignObject *obj, size_t id) override;
};

} // namespace RubyQml
