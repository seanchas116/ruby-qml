#pragma once

#include <qmlbind.h>
#include <ruby/ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

void rbqml_init_refcounter();

void rbqml_retain_value(VALUE value);
void rbqml_release_value(VALUE handle);

#ifdef __cplusplus
}
#endif
