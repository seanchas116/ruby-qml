#include <unordered_map>
#include <ruby/ruby.h>
#include "refcounter.h"

static std::unordered_map<VALUE, size_t> ref_counts;

static void mark_values(void *)
{
    for (auto &&kv : ref_counts) {
        rb_gc_mark(kv.first);
    }
}

static const rb_data_type_t marker_type = {
    "QML::RefCounterMarker",
    { &mark_values }
};
static VALUE marker_class;
static VALUE marker;

extern "C" {

void rbqml_init_refcounter()
{
    marker_class = rb_define_class_under(rb_path2class("QML"), "RefCounterMarker", rb_cObject);
    marker = TypedData_Wrap_Struct(marker_class, &marker_type, nullptr);
    rb_gc_register_address(&marker);
}

void rbqml_retain_value(VALUE value)
{
    if (ref_counts.find(value) == ref_counts.end()) {
        ref_counts[value] = 1;
    } else {
        ++ref_counts[value];
    }
}

void rbqml_release_value(VALUE handle)
{
    if (ref_counts[handle] <= 1) {
        ref_counts.erase(handle);
    }
    --ref_counts[handle];
}

}
