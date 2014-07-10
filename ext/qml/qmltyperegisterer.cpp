#include "qmltyperegisterer.h"
#include "foreignmetaobject.h"
#include "accessobject.h"
#include <QtQml>

namespace RubyQml {

QmlTypeRegisterer::QmlTypeRegisterer(const SP<ForeignMetaObject> &metaObject, const std::function<void (void *)> &createFunc) :
    mMetaObject(metaObject),
    mCreateFunc(createFunc)
{
    mFactoryArgs[0] = &ffi_type_pointer;
    if (ffi_prep_cif(&mFactoryCif, FFI_DEFAULT_ABI, 1, &ffi_type_void, mFactoryArgs) != FFI_OK) {
        throw std::runtime_error("failed to prepare FFI call interface");
    }
    mFactoryClosure = (ffi_closure *)ffi_closure_alloc(sizeof(ffi_closure), (void **)(&mFactoryFunc));
    if (!mFactoryClosure) {
        throw std::runtime_error("failed to allocate FFI closure");
    }

    auto callback = [](ffi_cif *cif, void *ret, void **args, void *data) {
        Q_UNUSED(cif);
        Q_UNUSED(ret);
        auto self = (QmlTypeRegisterer *)(data);
        self->mCreateFunc(*(void **)(args[0]));
    };

    if (ffi_prep_closure_loc(mFactoryClosure, &mFactoryCif, callback, this, (void *)mFactoryFunc) != FFI_OK) {
        ffi_closure_free(mFactoryClosure);
        throw std::runtime_error("failed to prepare FFI closure");
    }
}

QmlTypeRegisterer::~QmlTypeRegisterer()
{
    ffi_closure_free(mFactoryClosure);
}

void QmlTypeRegisterer::registerType(const char *uri, int versionMajor, int versionMinor, const char *qmlName)
{
    QByteArray className;
    className += mMetaObject->className();
    className += "*";

    QByteArray listName;
    listName += "QQmlListProperty<";
    listName += mMetaObject->className();
    listName += ">";

    QQmlPrivate::RegisterType type = {
        0,
        qRegisterNormalizedMetaType<AccessWrapper *>(className),
        qRegisterNormalizedMetaType<QQmlListProperty<AccessWrapper> >(listName),
        sizeof(AccessWrapper), mFactoryFunc,
        QString(),

        uri, versionMajor, versionMinor, qmlName, mMetaObject.get(),

        0, 0,

        QQmlPrivate::StaticCastSelector<AccessWrapper,QQmlParserStatus>::cast(),
        QQmlPrivate::StaticCastSelector<AccessWrapper,QQmlPropertyValueSource>::cast(),
        QQmlPrivate::StaticCastSelector<AccessWrapper,QQmlPropertyValueInterceptor>::cast(),

        0, 0,

        0,
        0
    };

    QQmlPrivate::qmlregister(QQmlPrivate::TypeRegistration, &type);
}

} // namespace RubyQml
