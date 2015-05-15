#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>
#include <stdbool.h>

VALUE rbqml_to_ruby(qmlbind_value value);
qmlbind_value rbqml_to_qml(VALUE value);
