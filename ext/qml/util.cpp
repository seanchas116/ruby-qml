#include "util.h"
#include "conversion.h"
#include <QtCore/QString>

namespace RubyQml {

void fail(const char *errorClassName, const QString &message)
{
    auto msg = message.toUtf8();
    protect([&] {
        rb_raise(rb_path2class(errorClassName), "%s", msg.data());
    });
}

bool isKindOf(VALUE obj, VALUE klass)
{
    auto result = protect([&] {
        return rb_obj_is_kind_of(obj, klass);
    });
    return fromRuby<bool>(result);
}

}
