#pragma once
#include <QVariant>

namespace RubyQml {

class RubyValue;

namespace Ext {

class AnyWrapper
{
public:
    AnyWrapper(RubyValue self);
    static RubyValue create(const QVariant &value, void (*markFunction)(const QVariant &) = nullptr);
    static void defineClass();

    QVariant value() const { return mValue; }
    void gc_mark();

private:
    QVariant mValue;
    void (*mMarkFunction)(const QVariant &);
};

} // namespace Ext
} // namespace RubyQml
