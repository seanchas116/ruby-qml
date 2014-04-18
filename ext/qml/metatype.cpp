#include <QMetaType>

extern "C" {

const char *rbqml_metatype_name(int typeNum)
{
    return QMetaType::typeName(typeNum);
}

int rbqml_metatype_from_name(const char *name)
{
    return QMetaType::type(name);
}

}
