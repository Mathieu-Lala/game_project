#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct SpellFactory {
    enum ID { STICK_ATTACK, SWORD_ATTACK, FIREBALL };

    template<ID>
    static auto create(entt::registry &, entt::entity caster, const glm::dvec2 &direction) -> entt::entity;

    template<typename... Args>
    static auto create(ID spell, Args &&... args)
    {
#define MAP_SPELL(id) \
    case ID::id: create<ID::id>(std::forward<Args>(args)...); break;

        switch (spell) {
            MAP_SPELL(STICK_ATTACK);
            MAP_SPELL(SWORD_ATTACK);
            MAP_SPELL(FIREBALL);

        default: assert(false && "unknown spell. Did you forget to map the enum value to factory function ?");
        }
#undef MAP_SPELL
    }
};

#define DECL_SPEC(id) \
    template<>        \
    auto SpellFactory::create<SpellFactory::ID::id>(entt::registry &, entt::entity, const glm::dvec2 &)->entt::entity

DECL_SPEC(STICK_ATTACK);
DECL_SPEC(SWORD_ATTACK);
DECL_SPEC(FIREBALL);

#undef DECL_SPEC

} // namespace game
