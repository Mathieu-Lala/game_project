#pragma once

#include <fmt/format.h>
#include <entt/entt.hpp>

#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/Core.hpp"

namespace engine::helper {

inline std::uint32_t loadTexture(const std::string &path) {
	static engine::Core::Holder holder{};

	auto key = entt::hashed_string{fmt::format("resource/texture/identifier/{}", path).data()};
	const auto &resource = holder.instance->getCache<engine::Texture>()
		.load<engine::LoaderTexture>(key, path);

	return resource->id;
}

}
