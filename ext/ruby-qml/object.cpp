#include <QObject>

namespace {

class QtMetaObjectGetter : public QObject
{
public:
    static const QMetaObject *qtMetaObject()
    {
        return &staticQtMetaObject;
    }
};

}

extern "C" {

const QMetaObject *qt_static_metaobject()
{
    return QtMetaObjectGetter::qtMetaObject();
}

const QMetaObject *qobject_static_metaobject()
{
    return &QObject::staticMetaObject;
}

const QMetaObject *qobject_metaobject(const QObject *obj)
{
    return obj->metaObject();
}

void qobject_destroy(QObject *obj)
{
    if (!obj->parent()) {
        obj->deleteLater();
    }
}

}
