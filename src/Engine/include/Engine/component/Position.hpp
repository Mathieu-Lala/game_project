#pragma once

#include <concepts>

#include <nlohmann/json.hpp>

namespace engine {

namespace d2 {

template<typename T> requires std::floating_point<T> || std::integral<T>
struct PositionT {
    using type = T;
    T x;
    T y;
};

using Position = PositionT<double>;

template<typename T>
void to_json(nlohmann::json &j, const engine::d2::PositionT<T> &pos)
{
    j["x"] = pos.x;
    j["y"] = pos.y;
}

template<typename T>
void from_json(const nlohmann::json &j, engine::d2::PositionT<T> &pos)
{
    pos.x = j.at("x");
    pos.y = j.at("y");
}

} // namespace d2

namespace d3 {

template<std::floating_point T>
struct PositionT {
    T x;
    T y;
    T z;
};

using Position = PositionT<double>;

} // namespace d3

} // namespace engine
