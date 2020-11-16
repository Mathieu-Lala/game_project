#pragma once

namespace engine {

// https://en.cppreference.com/w/cpp/utility/variant/visit
// cppcheck-suppress syntaxError
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// cppcheck-suppress syntaxError
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

} // namespace engine
