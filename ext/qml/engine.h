#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_cEngine;

VALUE rbqml_value_for_engine(qmlbind_engine engine);
qmlbind_engine rbqml_get_engine(VALUE value);

void rbqml_init_engine(void);
