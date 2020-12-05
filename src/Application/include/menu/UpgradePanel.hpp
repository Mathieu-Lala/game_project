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
    struct ClassTreeNode {
        int selfIndex;

        const Class *cl;
        ImVec2 relPos; // Position of the center of the element relative to tree bounds
        std::vector<ClassTreeNode> children;
    };

private:

    void processInputs(entt::registry &world, ThePURGE &game);

    void drawTree(entt::registry &world, ThePURGE &game) noexcept;
    auto getTreeDrawPos(const ImVec2 &relPos, float elemSize) const noexcept -> ImVec2;

    void drawDetailPanel(entt::registry &world, ThePURGE &game) noexcept;

    void updateClassTree(entt::registry &world, ThePURGE &game);
    auto generateTreeRec(ThePURGE &game, const Class *cl, int selfIndex = 0, int depth = 0) const noexcept -> ClassTreeNode;
    auto findInTree(const Class *cl) const noexcept -> const ClassTreeNode *;
    auto findParent(const Class *cl) const noexcept -> const ClassTreeNode *;

    auto isOwned(const Class *) const noexcept -> bool;
    auto isPurchaseable(const Class *) const noexcept -> bool;
    auto isUnavailable(const Class *) const noexcept -> bool;

    void printClassTreeDebug() const noexcept;
private:
    GUITexture m_static_background;
    GUITexture m_bind_popup;
    entt::entity m_player;

    const SpellData * m_spellBeingAssigned;

    const ClassTreeNode *m_selection;
    ImVec2 m_cursorCurrentPos; // in tree unit (@see `ClassTreeNode`)
    ImVec2 m_cursorDestinationPos;

    // Classes by tier
    std::vector<std::vector<const Class *>> m_classes;

    std::vector<const Class *> m_owned;
    std::vector<const Class *> m_purchaseable;

    ClassTreeNode m_root;
private:
    static inline constexpr auto kHeightMargins = 100; 
    static inline constexpr auto kWidthMargins = 150;

    static inline const ImVec2 treeTopLeft{524 + kWidthMargins, 223 + kHeightMargins};
    static inline const ImVec2 treeSize{1344 - 2 * kWidthMargins, 835 - 2 * kHeightMargins};

    static inline constexpr auto kIconSize = 100;
    static inline constexpr auto kFrameSize = 250;
    static inline constexpr auto kCursorSize = 200;

    static inline constexpr float kSelectionAnimationSpeed = 0.5f;
};

} // namespace game
