#pragma once

#include "qml.h"

extern VALUE rbqml_mMetaObject;

VALUE rbqml_metaobject_new(qmlbind_metaobject metaobj);
void rbqml_init_meta_object(void);
