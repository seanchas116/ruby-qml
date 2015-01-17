#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>
#include <stdbool.h>

VALUE rbqml_js_object_new(VALUE klass, qmlbind_value value, VALUE engine);

bool rbqml_js_object_p(VALUE value);
qmlbind_value rbqml_js_object_get(VALUE jsobject);
VALUE rbqml_js_object_get_engine(VALUE jsobject);

extern VALUE rbqml_cJSObject;

void rbqml_init_js_object(void);
