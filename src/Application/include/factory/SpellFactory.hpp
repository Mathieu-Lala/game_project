#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct SpellFactory {
    enum ID {
        SHOVEL_ATTAK,
        SWORD_ATTACK,
        FIREBALL,
        PIERCING_ARROW
    };

    template<ID>
    static auto create(entt::registry &, entt::entity caster, const glm::dvec2 &direction) -> entt::entity;

    template<typename... Args>
    static auto create(ID spell, Args &&... args)
    {
#define MAP_SPELL(id) \
    case ID::id: create<ID::id>(std::forward<Args>(args)...); break;

        switch (spell) {
            MAP_SPELL(SHOVEL_ATTAK);
            MAP_SPELL(SWORD_ATTACK);
            MAP_SPELL(FIREBALL);
            MAP_SPELL(PIERCING_ARROW);

        default: assert(false && "unknown spell. Did you forget to map the enum value to factory function ?");
        }
#undef MAP_SPELL
    }
};

#define DECL_SPEC(id) \
    template<>        \
    auto SpellFactory::create<SpellFactory::ID::id>(entt::registry &, entt::entity, const glm::dvec2 &)->entt::entity

DECL_SPEC(SHOVEL_ATTAK);
DECL_SPEC(SWORD_ATTACK);
DECL_SPEC(FIREBALL);
DECL_TYPE(PIERCING_ARROW);

#undef DECL_SPEC

} // namespace game
