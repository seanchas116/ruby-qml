#pragma once
#include "common.h"
#include <ffi.h>
#include <functional>

namespace RubyQml {

class ForeignMetaObject;

class QmlTypeRegisterer
{
public:
    using FactoryFunction = void (*)(void *);

    QmlTypeRegisterer(const SP<ForeignMetaObject> &metaObject, const std::function<void(void *)> &createFunc);
    ~QmlTypeRegisterer();

    void registerType(const char *uri, int versionMajor, int versionMinor, const char *qmlName);

private:

    SP<ForeignMetaObject> mMetaObject;
    std::function<void (void *)> mCreateFunc;
    ffi_type *mFactoryArgs[1];
    ffi_cif mFactoryCif;
    ffi_closure *mFactoryClosure = nullptr;
    FactoryFunction mFactoryFunc = nullptr;
};

} // namespace RubyQml
