#pragma once
#include <QtCore/QMetaType>
#include "valuereference.h"

namespace RubyQml {

class ObjectData
{
public:
    ObjectData(RubyValue rubyObject);

    RubyValue rubyObject() { return mRubyObject; }

    static std::shared_ptr<ObjectData> get(QObject *obj);
    static void set(QObject *obj, const std::shared_ptr<ObjectData> &data);

private:

    RubyValue mRubyObject;
};

} // namespace RubyQml

Q_DECLARE_METATYPE(std::shared_ptr<RubyQml::ObjectData>)

