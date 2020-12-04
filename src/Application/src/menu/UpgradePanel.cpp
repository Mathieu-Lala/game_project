#include <algorithm>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/resources/Texture.hpp>
#include <Engine/helpers/ImGui.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"


#include "menu/UpgradePanel.hpp"
#include "widgets/Fonts.hpp"
#include "widgets/GameHUD.hpp"

#include "ThePURGE.hpp"

void game::menu::UpgradePanel::create(entt::registry &world, ThePURGE &game)
{
    m_static_background = GUITexture{
        helper::getTexture("menus/upgrade_panel/static_background.png"),
        ImVec2(0, 0),
        ImVec2(1, 1),
    };

    auto player = game.player;

    const auto &ownedClasses = world.get<Classes>(player).ids;

    m_selectedClass = game.dbClasses().getByName(ownedClasses.back());

    updateClassTree(world, game);
}

void game::menu::UpgradePanel::draw(entt::registry &world, ThePURGE &game)
{
    static auto holder = engine::Core::Holder{};

    // auto player = game.player;

    // const auto &ownedClasses = world.get<Classes>(player).ids;
    // const auto skillPoints = world.get<SkillPoint>(player).count;


    GameHUD::draw(game, world);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(helper::frac2pixel({1.f, 1.f}));
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin("UpgradePanel", nullptr, ImGuiWindowFlags_NoDecoration);

    helper::drawTexture(m_static_background);

    drawTree(world, game);
    drawDetailPanel(world, game);

    ImGui::End();
}

void game::menu::UpgradePanel::drawDetailPanel(entt::registry &world, ThePURGE &game) noexcept
{
    auto player = game.player;

    const auto skillPoints = world.get<SkillPoint>(player).count;
    const auto &selectedClassSpell = game.dbSpells().db.at(m_selectedClass->spells.front());

    const GUITexture portrait{
        helper::getTexture(m_selectedClass->iconPath),
        helper::from1080p(58, 226),
        helper::from1080p(158, 179),
    };
    const GUITexture spellPortrait{
        helper::getTexture(selectedClassSpell.iconPath),
        helper::from1080p(64, 416),
        helper::from1080p(100, 100),
    };
    const GUITexture btn_buy{
        helper::getTexture("menus/upgrade_panel/button/buy.png"),
        helper::from1080p(119, 855),
        helper::from1080p(317, 197),
    };
    const GUITexture btn_cant{
        helper::getTexture("menus/upgrade_panel/button/cant.png"),
        helper::from1080p(119, 855),
        helper::from1080p(317, 197),
    };
    const GUITexture btn_alreadyowned{
        helper::getTexture("menus/upgrade_panel/button/owned.png"),
        helper::from1080p(119, 855),
        helper::from1080p(317, 197),
    };


    helper::drawTexture(portrait);
    helper::drawText(
        helper::frac2pixel(helper::from1080p(232, 295)), m_selectedClass->name, ImVec4(1, 1, 1, 1), Fonts::kimberley_50);

    helper::drawTexture(spellPortrait);
    helper::drawText(
        helper::frac2pixel(helper::from1080p(232, 448)), selectedClassSpell.name, ImVec4(1, 1, 1, 1), Fonts::opensans_44);

    helper::drawTextWrapped(
        helper::frac2pixel(helper::from1080p(67, 566)), selectedClassSpell.description, 510, Fonts::opensans_32);

    const ImVec4 bonusColor(0.16f, 07, 0, 1);
    const ImVec4 malusColor(0.7f, 0, 0, 1);

    helper::drawText(
        helper::frac2pixel(helper::from1080p(222, 747)),
        fmt::format("{:+}", m_selectedClass->health),
        m_selectedClass->health > 0 ? bonusColor : malusColor,
        Fonts::kimberley_35);
    helper::drawText(
        helper::frac2pixel(helper::from1080p(222, 792)),
        fmt::format("{:+}", m_selectedClass->speed),
        m_selectedClass->speed > 0 ? bonusColor : malusColor,
        Fonts::kimberley_35);


    if (isOwned(m_selectedClass)) {
        helper::drawTexture(btn_alreadyowned);
    } else {
        if (isPurchaseable(m_selectedClass) && skillPoints >= m_selectedClass->cost)
            helper::drawTexture(btn_buy);
        else
            helper::drawTexture(btn_cant);

        helper::drawText(
            helper::frac2pixel(helper::from1080p(269, 960)),
            std::to_string(m_selectedClass->cost),
            ImVec4(0, 0, 0, 1),
            Fonts::kimberley_62);
    }
}

void game::menu::UpgradePanel::drawTree(entt::registry &, ThePURGE &) noexcept
{
    std::vector<const ClassTreeNode *> todo = {&m_root};

    while (!todo.empty()) {
        const auto current = todo.back();
        todo.pop_back();

        for (const auto &child : current->children) {
            ImGui::GetWindowDrawList()->AddLine(
                getTreeDrawPos(current->relPos, 0), getTreeDrawPos(child.relPos, 0), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), 3.f);
            todo.push_back(&child);
        }

        std::uint32_t glowTexture;

        if (isOwned(current->cl))
            glowTexture = helper::getTexture("menus/upgrade_panel/tree/frames/owned.png");
        else if (isPurchaseable(current->cl))
            glowTexture = helper::getTexture("menus/upgrade_panel/tree/frames/buyable.png");
        else
            glowTexture = helper::getTexture("menus/upgrade_panel/tree/frames/unavailable.png");

        helper::drawTexture(glowTexture, getTreeDrawPos(current->relPos, kFrameSize), ImVec2(kFrameSize, kFrameSize));

        const auto iconTexture = helper::getTexture(current->cl->iconPath);
        helper::drawTexture(iconTexture, getTreeDrawPos(current->relPos, kIconSize), ImVec2(kIconSize, kIconSize));
    }

    m_treeSelectionCurrentPos.x = std::lerp(m_treeSelectionCurrentPos.x, m_treeSelectionDestinationPos.x, kSelectionAnimationSpeed);
    m_treeSelectionCurrentPos.y = std::lerp(m_treeSelectionCurrentPos.y, m_treeSelectionDestinationPos.y, kSelectionAnimationSpeed);

    const auto cursorTexture = helper::getTexture("menus/upgrade_panel/tree/cursor.png");
    helper::drawTexture(cursorTexture, getTreeDrawPos(m_treeSelectionCurrentPos, kCursorSize), ImVec2(kCursorSize, kCursorSize));
}

auto game::menu::UpgradePanel::getTreeDrawPos(const ImVec2 &relPos, float elemSize) const noexcept -> ImVec2
{
    const ImVec2 centerAs1080p(treeTopLeft.x + treeSize.x * relPos.x, treeTopLeft.y + treeSize.y * relPos.y);

    return helper::frac2pixel(helper::from1080p(centerAs1080p.x - elemSize * 0.5f, centerAs1080p.y - elemSize * 0.5f));
}

void game::menu::UpgradePanel::updateClassTree(entt::registry &world, ThePURGE &game)
{
    const auto player = game.player;

    m_owned.clear();
    m_classes.clear();
    m_purchaseable.clear();

    for (const auto &name : world.get<Classes>(player).ids) m_owned.push_back(game.dbClasses().getByName(name));


    m_classes.push_back({&game.dbClasses().getStarterClass()});

    for (int i = 0;; ++i) {
        const auto &lastRow = m_classes.back();
        std::vector<const Class *> thisRow;

        for (const auto *parent : lastRow) {
            for (const auto &childName : parent->children) {
                const auto *child = game.dbClasses().getByName(childName);
                thisRow.push_back(child);

                if (std::find(std::begin(m_owned), std::end(m_owned), parent) != std::end(m_owned))
                    m_purchaseable.push_back(child);
            }
        }

        if (thisRow.empty()) break;

        m_classes.push_back(std::move(thisRow));
    }

    printClassTreeDebug();

    m_root = generateTreeRec(game, m_classes[0][0]);

    m_treeSelectionDestinationPos = findInTree(m_selectedClass)->relPos;
    m_treeSelectionCurrentPos = m_treeSelectionDestinationPos;
}

auto game::menu::UpgradePanel::generateTreeRec(ThePURGE &game, const Class *cl, int depth) const noexcept -> ClassTreeNode
{
    float yPos = static_cast<float>(depth) / (m_classes.size() - 1);

    if (depth == m_classes.size() - 1) {
        const auto &lastRow = m_classes.back();
        const auto iter = std::find(std::begin(lastRow), std::end(lastRow), cl);
        auto idx = std::distance(std::begin(lastRow), iter);

        float xPos = static_cast<float>(idx) / (lastRow.size() - 1);

        return ClassTreeNode{.cl = cl, .relPos = ImVec2(xPos, yPos), .children = {}};
    }

    ClassTreeNode result;

    result.cl = cl;
    result.relPos.y = yPos;
    result.relPos.x = 0;

    for (const auto &childName : cl->children) {
        result.children.push_back(generateTreeRec(game, game.dbClasses().getByName(childName), depth + 1));
        result.relPos.x += result.children.back().relPos.x / cl->children.size();
    }

    return result;
}

auto game::menu::UpgradePanel::findInTree(const Class *cl) const noexcept -> const ClassTreeNode *
{
    std::vector<const ClassTreeNode *> todo = {&m_root};

    while (!todo.empty()) {
        const auto current = todo.back();
        if (current->cl == cl) return current;

        todo.pop_back();

        for (const auto &child : current->children) todo.push_back(&child);
    }

    assert(!"The requested class does not exist in the tree");
    return nullptr; // guaranteed crash
}

bool game::menu::UpgradePanel::isOwned(const Class *cl) const noexcept
{
    return std::find(std::begin(m_owned), std::end(m_owned), cl) != std::end(m_owned);
}

auto game::menu::UpgradePanel::isPurchaseable(const Class *cl) const noexcept -> bool
{
    return std::find(std::begin(m_purchaseable), std::end(m_purchaseable), cl) != std::end(m_purchaseable);
}

void game::menu::UpgradePanel::printClassTreeDebug() const noexcept
{
    spdlog::info("================ CLASS TREE DEBUG ================");
    spdlog::info("Tree :");

    for (int i = 0; const auto &tier : m_classes) {
        spdlog::info(" Tier {} :", ++i);

        for (const auto *cl : tier) spdlog::info("\t{}", cl->name);
    }

    spdlog::info("");
    spdlog::info("Owned :");
    for (const auto *owned : m_owned) spdlog::info("\t{}", owned->name);

    spdlog::info("");
    spdlog::info("Purchaseable :");
    for (const auto *purchaseable : m_purchaseable) spdlog::info("\t{}", purchaseable->name);


    spdlog::info("==================================================");
}


void game::menu::UpgradePanel::event(entt::registry &, ThePURGE &game, const engine::Event &e)
{
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_P: game.setMenu(nullptr); break;
                default: break;
                }
            },
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::CENTER2: game.setMenu(nullptr); break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
}