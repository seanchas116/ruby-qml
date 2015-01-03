#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>
#include <stdbool.h>

typedef struct {
    qmlbind_value value;
} rubyqml_js_object_t;

VALUE rubyqml_js_object_alloc(VALUE klass);

bool rubyqml_is_js_object(VALUE value);
qmlbind_value rubyqml_js_object_get(VALUE jsobject);
void rubyqml_js_object_set(VALUE jsobject, qmlbind_value value);

VALUE rubyqml_js_object_class(void);

void rubyqml_init_js_object(void);
