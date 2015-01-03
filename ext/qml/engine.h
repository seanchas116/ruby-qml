#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

qmlbind_engine rubyqml_global_engine(void);
VALUE rubyqml_global_engine_value(void);

void rubyqml_init_engine(void);
