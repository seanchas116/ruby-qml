#pragma once
#include <QVariant>

namespace RubyQml {

class RubyValue;

class Ext_AnyWrapper
{
public:
    Ext_AnyWrapper(RubyValue self);
    static RubyValue create(const QVariant &value, void (*markFunction)(const QVariant &) = nullptr);
    static void defineClass();

    QVariant value() const { return mValue; }
    void gc_mark();

private:
    QVariant mValue;
    void (*mMarkFunction)(const QVariant &);
};

} // namespace RubyQml
