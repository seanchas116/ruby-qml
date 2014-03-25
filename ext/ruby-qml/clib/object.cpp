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

const QMetaObject *rbqml_qt_metaobject()
{
    return QtMetaObjectGetter::qtMetaObject();
}

const QMetaObject *rbqml_object_static_metaobject()
{
    return &QObject::staticMetaObject;
}

const QMetaObject *rbqml_object_metaobject(const QObject *obj)
{
    return obj->metaObject();
}

void rbqml_object_destroy(QObject *obj)
{
    if (!obj->parent()) {
        obj->deleteLater();
    }
}

}
