#include "application.h"
#include "component.h"
#include "engine.h"
#include "type_register.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"

void Init_qml(void)
{
    rubyqml_init_application();
    rubyqml_init_engine();
    rubyqml_init_js_object();
    rubyqml_init_js_array();
    rubyqml_init_js_function();
}
