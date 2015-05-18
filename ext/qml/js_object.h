#pragma once

#include "qml.h"
#include <stdbool.h>

qmlbind_value rbqml_get_property(qmlbind_value obj, const char *key);
qmlbind_value rbqml_get_array_item(qmlbind_value obj, int index);
void rbqml_set_property(qmlbind_value obj, const char *key, qmlbind_value value);
void rbqml_set_array_item(qmlbind_value obj, int index, qmlbind_value value);
qmlbind_value rbqml_get_iterator_value(qmlbind_iterator iter);

VALUE rbqml_js_object_new(VALUE klass, qmlbind_value value);

bool rbqml_js_object_p(VALUE value);
qmlbind_value rbqml_js_object_get(VALUE jsobject);

extern VALUE rbqml_cJSObject;

void rbqml_init_js_object(void);
