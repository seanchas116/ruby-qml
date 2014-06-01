#pragma once
#include <memory>

namespace RubyQml {

template <typename T>
using SP = std::shared_ptr<T>;

template <typename T>
using WP = std::weak_ptr<T>;

template <typename T, typename ... Args>
inline SP<T> makeSP(Args && ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace RubyQml
