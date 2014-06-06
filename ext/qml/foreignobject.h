#pragma once
#include <QObject>
#include "common.h"

namespace RubyQml {

class ForeignClass;

class ForeignObject : public QObject
{
public:
    explicit ForeignObject(const SP<ForeignClass> &foreignClass, QObject *parent = 0);

    SP<ForeignClass> foreignClass() const { return mForeignClass; }

    const QMetaObject *metaObject() const override;
    int qt_metacall(QMetaObject::Call call, int index, void **argv) override;

private:
    SP<ForeignClass> mForeignClass;
};

} // namespace RubyQml
