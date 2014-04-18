#include "backendmain.h"
#include "util.h"
#include <QtCore/QDebug>

namespace RubyQml {

BackendMain::BackendMain(QObject *parent) :
    QObject(parent)
{
    if (sInstance) {
        qWarning() << "Duplicate BackendMain instance";
    } else {
        sInstance = this;
    }

    // initialize metatypes
    qRegisterMetaType<const QMetaObject*>();
}

BackendMain *BackendMain::sInstance = nullptr;

} // namespace RubyQml

using namespace RubyQml;

extern "C" {

QObject *rbqml_backendmain_instance()
{
    return BackendMain::instance();
}

const QMetaObject *rbqml_backendmain_static_metaobject()
{
    return &RubyQml::BackendMain::staticMetaObject;
}

}
