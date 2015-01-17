#include "application.h"
#include "component.h"
#include "engine.h"
#include "type_register.h"
#include "js_object.h"
#include "js_array.h"
#include "js_function.h"

void Init_qml(void)
{
    rbqml_init_application();
    rbqml_init_engine();
    rbqml_init_js_object();
    rbqml_init_js_array();
    rbqml_init_js_function();
}
