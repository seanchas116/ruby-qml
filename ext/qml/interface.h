#ifndef INTERFACE_H
#define INTERFACE_H

#include <ruby/ruby.h>
#include <qmlbind.h>

extern VALUE rbqml_mInterface;
qmlbind_interface rbqml_get_interface(void);

void rbqml_init_interface(void);

#endif // INTERFACE_H
