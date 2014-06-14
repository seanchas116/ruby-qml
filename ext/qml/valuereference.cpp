#include "valuereference.h"
#include <QSet>

namespace RubyQml {

using ReferenceCounts = QHash<RubyValue, int>;
Q_GLOBAL_STATIC(ReferenceCounts, referenceCounts)

struct ValueReference::Owner
{
    Owner(RubyValue value) :
        value(value)
    {
        auto &refCounts = *referenceCounts;
        if (!refCounts.contains(value)) {
            refCounts[value] = 0;
        }
        refCounts[value] += 1;
    }
    ~Owner()
    {
        auto &refCounts = *referenceCounts;
        refCounts[value] -= 1;
        if (refCounts[value] == 0) {
            refCounts.remove(value);
        }
    }
    RubyValue value;
};

ValueReference::ValueReference(RubyValue value) :
    mOwner(makeSP<Owner>(value))
{
}

RubyValue ValueReference::value() const
{
    return mOwner->value;
}

void ValueReference::markAllReferences()
{
    auto begin = referenceCounts->begin();
    auto end = referenceCounts->end();
    for (auto i = begin; i != end; ++i) {
        rb_gc_mark(i.key());
    }
}

} // namespace RubyQml
