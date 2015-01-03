#include "application.h"

static qmlbind_application application;

qmlbind_application rubyqml_application(void)
{
    return application;
}

void rubyqml_init_application(void)
{
    VALUE argv = rb_get_argv();
    int len = RARRAY_LEN(argv);
    char **strs = malloc(len * sizeof(char *));

    for (int i = 0; i < len; ++i) {
        VALUE arg = RARRAY_AREF(argv, i);
        strs[i] = rb_string_value_cstr(&arg);
    }

    application = qmlbind_application_new(len, strs);
}
