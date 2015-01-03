#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

qmlbind_application rubyqml_application(void);
void rubyqml_init_application(void);
