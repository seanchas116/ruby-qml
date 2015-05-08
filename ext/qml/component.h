#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_cComponent;

qmlbind_component rbqml_get_component(VALUE value);

void rbqml_init_component(void);
