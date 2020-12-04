#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "AMenu.hpp"
#include "widgets/helpers.hpp"

namespace game::menu {

class UpgradePanel : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    void drawTree(entt::registry &world, ThePURGE &game) noexcept;
    void drawDetailPanel(entt::registry &world, ThePURGE &game) noexcept;

    void updateClassTree(entt::registry &world, ThePURGE &game);

    auto isOwned(const Class *) const noexcept -> bool;
    auto isPurchaseable(const Class *) const noexcept -> bool;

    void printClassTreeDebug() const noexcept;
private:
    GUITexture m_static_background;


    const Class *m_selectedClass;

    // Classes by tier
    std::vector<std::vector<const Class *>> m_classes;

    std::vector<const Class *> m_owned;
    std::vector<const Class *> m_purchaseable;
};

} // namespace game
