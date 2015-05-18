#pragma once

#include "qml.h"

extern VALUE rbqml_cApplication;

qmlbind_application rbqml_get_application(VALUE self);

void rbqml_init_application(void);
