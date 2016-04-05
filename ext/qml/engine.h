#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_cEngine;

qmlbind_engine *rbqml_get_engine(VALUE value);

void rbqml_init_engine(void);
