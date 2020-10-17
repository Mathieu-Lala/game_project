#include "Engine/component/Color.hpp"
#include "Engine/resources/LoaderColor.hpp"

auto engine::LoaderColor::load(glm::vec3 &&color) const -> std::shared_ptr<Color>
{
    return std::shared_ptr<Color>(new Color{Color::ctor(std::move(color))}, Color::dtor);
}
