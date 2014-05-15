#pragma once
#include <QtCore/QMetaType>
#include <ruby.h>

namespace RubyQml {

class ObjectData
{
public:
    ObjectData(VALUE rubyObject);
    ~ObjectData();

    VALUE rubyObject() { return mRubyObject; }

    static std::shared_ptr<ObjectData> get(QObject *obj);
    static void set(QObject *obj, const std::shared_ptr<ObjectData> &data);

private:

    VALUE mRubyObject = Qnil;
};

} // namespace RubyQml

Q_DECLARE_METATYPE(std::shared_ptr<RubyQml::ObjectData>)

