#include "accessclass.h"
#include "accessobject.h"
#include "util.h"
#include "conversion.h"

namespace RubyQml {

AccessClass::AccessClass(VALUE className, VALUE methodInfos, VALUE signalInfos, VALUE propertyInfos)
{
    setClassName(fromRuby<QByteArray>(className));
    protect([&] {
        rb_check_array_type(methodInfos);
        rb_check_array_type(signalInfos);
        rb_check_array_type(propertyInfos);
    });
    for (int i = 0; i < RARRAY_LEN(methodInfos); ++i) {
        auto info = RARRAY_AREF(methodInfos, i);
        auto nameSym = send(info, "name");
        auto name = fromRuby<QByteArray>(nameSym);
        auto params = fromRuby<QList<QByteArray>>(send(info, "params"));
        addMethod(name, SYM2ID(nameSym), params);
    }
    for (int i = 0; i < RARRAY_LEN(signalInfos); ++i) {
        auto info = RARRAY_AREF(signalInfos, i);
        auto nameSym = send(info, "name");
        auto name = fromRuby<QByteArray>(nameSym);
        auto params = fromRuby<QList<QByteArray>>(send(info, "params"));
        addSignal(name, SYM2ID(nameSym), params);
    }
    for (int i = 0; i < RARRAY_LEN(propertyInfos); ++i) {
        auto info = RARRAY_AREF(propertyInfos, i);
        auto getterSym = send(info, "getter");
        auto setterSym = send(info, "setter");
        auto notifierSym = send(info, "notifier");
        auto name = fromRuby<QByteArray>(send(info, "name"));
        addProperty(name, SYM2ID(getterSym), SYM2ID(setterSym), Property::Flag::Readable | Property::Flag::Writable, true, SYM2ID(notifierSym));
    }
}

QVariant AccessClass::callMethod(ForeignObject *obj, size_t id, const QVariantList &args)
{
    auto self = static_cast<AccessObject *>(obj)->value();
    QVariant ret;
    withGvl([&] {
        std::vector<VALUE> values(args.size());
        std::transform(args.begin(), args.end(), values.begin(), toRuby<QVariant>);
        VALUE retValue = Qnil;
        protect([&] {
            rescue([&] {
                retValue = rb_funcallv(self, id, values.size(), values.data());
            }, [&](VALUE excObject) {
                rb_funcall(rb_path2class("QML::Application"), rb_intern("notify_error"), 1, excObject);
            });
        });
        ret = fromRuby<QVariant>(retValue);
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
