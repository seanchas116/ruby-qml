#include "accessclass.h"
#include "accessobject.h"
#include "util.h"
#include <QDebug>

namespace RubyQml {

AccessClass::AccessClass(RubyValue className, RubyValue methodInfos, RubyValue signalInfos, RubyValue propertyInfos)
{
    setClassName(className.to<QByteArray>());
    protect([&] {
        rb_check_array_type(methodInfos);
        rb_check_array_type(signalInfos);
        rb_check_array_type(propertyInfos);
    });
    for (int i = 0; i < RARRAY_LEN(VALUE(methodInfos)); ++i) {
        RubyValue info = RARRAY_AREF(VALUE(methodInfos), i);
        auto nameSym = info.send("name");
        addMethod(nameSym.to<QByteArray>(),
                  nameSym.toID(),
                  info.send("params").to<QList<QByteArray>>());
    }
    for (int i = 0; i < RARRAY_LEN(VALUE(signalInfos)); ++i) {
        RubyValue info = RARRAY_AREF(VALUE(signalInfos), i);
        auto nameSym = info.send("name");
        addSignal(nameSym.to<QByteArray>(),
                  nameSym.toID(),
                  info.send("params").to<QList<QByteArray>>());
    }
    for (int i = 0; i < RARRAY_LEN(VALUE(propertyInfos)); ++i) {
        RubyValue info = RARRAY_AREF(VALUE(propertyInfos), i);
        addProperty(info.send("name").to<QByteArray>(),
                    info.send("getter").toID(),
                    info.send("setter").toID(),
                    Property::Flag::Readable | Property::Flag::Writable,
                    true,
                    info.send("notifier").toID());
    }
}

QVariant AccessClass::callMethod(ForeignObject *obj, size_t id, const QVariantList &args)
{
    auto self = static_cast<AccessWrapper *>(obj)->wrappedValue();
    QVariant ret;
    withGvl([&] {
        std::vector<VALUE> values(args.size());
        std::transform(args.begin(), args.end(), values.begin(), &RubyValue::from<QVariant>);
        RubyValue retValue;
        protect([&] {
            rescue([&] {
                retValue = rb_funcallv(self, id, values.size(), values.data());
            }, [&](RubyValue excObject) {
                rb_funcall(rb_path2class("QML::Application"), rb_intern("notify_error"), 1, excObject);
            });
        });
        ret = retValue.to<QVariant>();
    });
    return ret;
}

void AccessClass::setProperty(ForeignObject *obj, size_t id, const QVariant &variant)
{
    callMethod(obj, id, {variant});
}

QVariant AccessClass::getProperty(ForeignObject *obj, size_t id)
{
    return callMethod(obj, id, {});
}

} // namespace RubyQml
