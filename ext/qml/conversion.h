#pragma once

#include <ruby/ruby.h>
#include <qmlbind.h>
#include <stdbool.h>

VALUE rubyqml_to_ruby(qmlbind_value value);
qmlbind_value rubyqml_to_qml(VALUE value);
