#pragma once

namespace RubyQml {

template <typename TFunction, TFunction function>
struct FunctionInfo {};

#define RUBYQML_FUNCTION_INFO(func) FunctionInfo<decltype(func), func>()

template <typename TMemberFunction, TMemberFunction memberFunction>
struct MemberFunctionInfo {};

#define RUBYQML_MEMBER_FUNCTION_INFO(memfn) MemberFunctionInfo<decltype(memfn), memfn>()


} // namespace RubyQml
