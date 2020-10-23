#include "Engine/component/Color.hpp"
#include "Engine/resources/LoaderColor.hpp"
#include "Engine/component/Texture.hpp"
#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/resources/AudioFileLoader.hpp"
#include "Engine/audio/AudioManager.hpp"

auto engine::LoaderColor::load(glm::vec3 &&color) const -> std::shared_ptr<Color>
{
    return std::shared_ptr<Color>(new Color{Color::ctor(std::move(color))}, Color::dtor);
}

auto engine::LoaderTexture::load(const std::string_view path, const std::array<float, 4ul> &clip) const
    -> std::shared_ptr<Texture>
{
    return std::shared_ptr<Texture>(new Texture{Texture::ctor(path, clip)}, Texture::dtor);
}

auto engine::AudioFileLoader::load(const std::string &path) const -> std::shared_ptr<AudioFileBuffer>
{
    return std::make_shared<AudioFileBuffer>(path);
}
