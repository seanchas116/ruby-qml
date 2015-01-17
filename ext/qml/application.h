#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_cApplication;
extern VALUE rbqml_application_instance;

void rbqml_init_application(void);
