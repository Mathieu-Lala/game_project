#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct SpellFactory {
    enum ID {
        SHOVEL_ATTACK,
        SWORD_ATTACK,
        FIREBALL,
        PIERCING_ARROW,

        ENEMY_ATTACK,
        DEBUG_GIANT_FIREBALL,
    };

    template<ID>
    static auto create(entt::registry &, entt::entity caster, const glm::dvec2 &direction) -> entt::entity;

    template<typename... Args>
    static auto create(ID spell, Args &&... args)
    {
#define MAP_SPELL(id) \
    case ID::id: create<ID::id>(std::forward<Args>(args)...); break;

        switch (spell) {
            MAP_SPELL(ENEMY_ATTACK);
            MAP_SPELL(SHOVEL_ATTACK);
            MAP_SPELL(SWORD_ATTACK);
            MAP_SPELL(FIREBALL);
            MAP_SPELL(PIERCING_ARROW);

            MAP_SPELL(DEBUG_GIANT_FIREBALL);

        default: assert(false && "unknown spell. Did you forget to map the enum value to factory function ?");
        }
#undef MAP_SPELL
    }

    template<typename... Args>
    static auto create(const std::string_view search, Args &&... args)
    {
        struct s {
            std::string tag;
            ID id;
        };
        const auto db = std::to_array<s>(
            {{"shovel", SHOVEL_ATTACK}, {"sword", SWORD_ATTACK}, {"fireball", FIREBALL}, {"arrow", PIERCING_ARROW}});

        create(
            std::find_if(db.begin(), db.end(), [&search](auto &i) { return i.tag == search; })->id,
            std::forward<Args>(args)...);
    }
};

#define DECL_SPEC(id) \
    template<>        \
    auto SpellFactory::create<SpellFactory::ID::id>(entt::registry &, entt::entity, const glm::dvec2 &)->entt::entity

DECL_SPEC(ENEMY_ATTACK);
DECL_SPEC(SHOVEL_ATTACK);
DECL_SPEC(SWORD_ATTACK);
DECL_SPEC(FIREBALL);
DECL_SPEC(DEBUG_GIANT_FIREBALL);
DECL_SPEC(PIERCING_ARROW);

#undef DECL_SPEC

} // namespace game
