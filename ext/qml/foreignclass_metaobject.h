#pragma once
#include <QMetaObject>
#include <QVector>
#include <QHash>
#include "common.h"
#include "foreignclass.h"

namespace RubyQml {

class ForeignClass::MetaObject : public QMetaObject
{
public:
    MetaObject(const SP<ForeignClass> &foreignClass);

    int dynamicMetaCall(Object *obj, QMetaObject::Call call, int index, void **argv);
    QHash<std::size_t, int> signalIndexHash() const { return mSignalIndexHash; }

private:
    class StringPool;

    void buildData();
    QVector<uint> writeMetaData(StringPool &stringPool);

    QVector<uint8_t> mStringData;
    QVector<uint> mData;

    WP<ForeignClass> mForeignClassWP;
    SP<MetaObject> mSuperMetaObject;

    QHash<std::size_t, int> mSignalIndexHash;

    QList<std::size_t> mMethodIds;
    QList<int> mMethodArities;
    int mMethodCount;
    int mSignalCount;

    QList<std::size_t> mPropertySetterIds;
    QList<std::size_t> mPropertyGetterIds;
    int mPropertyCount;
};

} // namespace RubyQml
