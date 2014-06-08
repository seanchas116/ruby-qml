#pragma once
#include <QObject>
#include "common.h"

namespace RubyQml {

class ForeignMetaObject;

class ForeignObject : public QObject
{
public:
    explicit ForeignObject(const SP<ForeignMetaObject> &mMetaObject, QObject *parent = 0);

    const QMetaObject *metaObject() const override;
    int qt_metacall(QMetaObject::Call call, int index, void **argv) override;

private:
    SP<ForeignMetaObject> mMetaObject;
};

} // namespace RubyQml
