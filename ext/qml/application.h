#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_cApplication;

qmlbind_application rbqml_get_application(VALUE self);

void rbqml_init_application(void);
