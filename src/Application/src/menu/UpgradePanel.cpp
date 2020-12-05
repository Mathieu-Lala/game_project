#include <algorithm>

#include <Engine/Event/Event.hpp>
#include <Engine/Event/JoystickManager.hpp>

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

    m_bind_popup = GUITexture{
        helper::getTexture("menus/upgrade_panel/key_assignment_popup.png"),
        ImVec2(0, 0),
        ImVec2(1, 1),
    };


    m_player = game.player;

    const auto &ownedClasses = world.get<Classes>(m_player).ids;

    updateClassTree(world, game);

    m_selection = findInTree(game.dbClasses().getByName(ownedClasses.back()));
    m_cursorDestinationPos = m_selection->relPos;
    m_cursorCurrentPos = m_cursorDestinationPos;
}

void game::menu::UpgradePanel::draw(entt::registry &world, ThePURGE &game)
{
    static auto holder = engine::Core::Holder{};

    processInputs(world, game);

    GameHUD::draw(game, world);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(helper::frac2pixel({1.f, 1.f}));
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin("UpgradePanel", nullptr, ImGuiWindowFlags_NoDecoration);

    helper::drawTexture(m_static_background);

    drawTree(world, game);
    drawDetailPanel(world, game);

    if (m_spellBeingAssigned) {
        helper::drawTexture(m_bind_popup);
        const auto spellIcon = helper::getTexture(m_spellBeingAssigned->iconPath);

        helper::drawTexture(
            spellIcon, helper::frac2pixel(helper::from1080p(885, 446)), helper::frac2pixel(helper::from1080p(150, 150)));
    }


    ImGui::End();
}

void game::menu::UpgradePanel::drawDetailPanel(entt::registry &world, ThePURGE &game) noexcept
{
    const auto skillPoints = world.get<SkillPoint>(m_player).count;
    const auto &selectedClassSpell = game.dbSpells().db.at(m_selection->cl->spells.front());

    const GUITexture portrait{
        helper::getTexture(m_selection->cl->iconPath),
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
        helper::frac2pixel(helper::from1080p(232, 295)), m_selection->cl->name, ImVec4(1, 1, 1, 1), Fonts::kimberley_50);

    helper::drawTexture(spellPortrait);
    helper::drawText(
        helper::frac2pixel(helper::from1080p(232, 448)), selectedClassSpell.name, ImVec4(1, 1, 1, 1), Fonts::opensans_44);

    helper::drawTextWrapped(
        helper::frac2pixel(helper::from1080p(67, 566)), selectedClassSpell.description, 510, Fonts::opensans_32);

    const ImVec4 bonusColor(0.16f, 07, 0, 1);
    const ImVec4 malusColor(0.7f, 0, 0, 1);

    helper::drawText(
        helper::frac2pixel(helper::from1080p(222, 747)),
        fmt::format("{:+}", m_selection->cl->health),
        m_selection->cl->health > 0 ? bonusColor : malusColor,
        Fonts::kimberley_35);
    helper::drawText(
        helper::frac2pixel(helper::from1080p(222, 792)),
        fmt::format("{:+}", m_selection->cl->speed),
        m_selection->cl->speed > 0 ? bonusColor : malusColor,
        Fonts::kimberley_35);


    if (isOwned(m_selection->cl)) {
        helper::drawTexture(btn_alreadyowned);
    } else {
        if (isPurchaseable(m_selection->cl) && skillPoints >= m_selection->cl->cost)
            helper::drawTexture(btn_buy);
        else
            helper::drawTexture(btn_cant);

        helper::drawText(
            helper::frac2pixel(helper::from1080p(269, 960)),
            std::to_string(m_selection->cl->cost),
            ImVec4(0, 0, 0, 1),
            Fonts::kimberley_62);
    }
}

void game::menu::UpgradePanel::processInputs(entt::registry &world, ThePURGE &game)
{
    if (m_spellBeingAssigned) return;


    const auto *parent = findParent(m_selection->cl);

    if (up() && parent) {
        m_selection = parent;
    } else if (down() && !m_selection->children.empty()) {
        for (const auto &child : m_selection->children) {
            if (!isUnavailable(child.cl)) {
                m_selection = &m_selection->children.front();
                break;
            }
        }
    } else if (left() && parent) {
        for (int i = m_selection->selfIndex - 1; i >= 0; --i) {
            const auto *it = &parent->children[i];
            if (!isUnavailable(it->cl)) {
                m_selection = it;
                break;
            }
        }
    } else if (right() && parent)
        for (int i = m_selection->selfIndex + 1; i < parent->children.size(); ++i) {
            const auto *it = &parent->children[i];
            if (!isUnavailable(it->cl)) {
                m_selection = it;
                break;
            }
        }

    m_cursorDestinationPos = m_selection->relPos;

    const auto sp = world.get<SkillPoint>(m_player).count;
    if (select() && isPurchaseable(m_selection->cl) && sp >= m_selection->cl->cost) {
        game.logics()->onPlayerPurchase.publish(world, m_player, *m_selection->cl);

        m_spellBeingAssigned = &game.dbSpells().db.at(m_selection->cl->spells.front());
        updateClassTree(world, game);
        m_selection = findInTree(game.dbClasses().getByName(world.get<Classes>(m_player).ids.back()));
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
                getTreeDrawPos(current->relPos, 0),
                getTreeDrawPos(child.relPos, 0),
                ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)),
                3.f);
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

    m_cursorCurrentPos.x = std::lerp(m_cursorCurrentPos.x, m_cursorDestinationPos.x, kSelectionAnimationSpeed);
    m_cursorCurrentPos.y = std::lerp(m_cursorCurrentPos.y, m_cursorDestinationPos.y, kSelectionAnimationSpeed);

    const auto cursorTexture = helper::getTexture("menus/upgrade_panel/tree/cursor.png");
    helper::drawTexture(cursorTexture, getTreeDrawPos(m_cursorCurrentPos, kCursorSize), ImVec2(kCursorSize, kCursorSize));
}

auto game::menu::UpgradePanel::getTreeDrawPos(const ImVec2 &relPos, float elemSize) const noexcept -> ImVec2
{
    const ImVec2 centerAs1080p(treeTopLeft.x + treeSize.x * relPos.x, treeTopLeft.y + treeSize.y * relPos.y);

    return helper::frac2pixel(helper::from1080p(centerAs1080p.x - elemSize * 0.5f, centerAs1080p.y - elemSize * 0.5f));
}

void game::menu::UpgradePanel::updateClassTree(entt::registry &world, ThePURGE &game)
{
    m_owned.clear();
    m_classes.clear();
    m_purchaseable.clear();

    for (const auto &name : world.get<Classes>(m_player).ids) m_owned.push_back(game.dbClasses().getByName(name));


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

    // remove owned classes from purchaseable
    m_purchaseable.erase(std::remove_if(std::begin(m_purchaseable), std::end(m_purchaseable), [this](const auto &c) { return std::find(std::begin(m_owned), std::end(m_owned), c) != std::end(m_owned); }), std::end(m_purchaseable));

    printClassTreeDebug();

    m_root = generateTreeRec(game, m_classes[0][0]);
}

auto game::menu::UpgradePanel::generateTreeRec(ThePURGE &game, const Class *cl, int selfIndex, int depth) const noexcept
    -> ClassTreeNode
{
    float yPos = static_cast<float>(depth) / (m_classes.size() - 1);

    if (depth == m_classes.size() - 1) {
        const auto &lastRow = m_classes.back();
        const auto iter = std::find(std::begin(lastRow), std::end(lastRow), cl);
        auto idx = std::distance(std::begin(lastRow), iter);

        float xPos = static_cast<float>(idx) / (lastRow.size() - 1);

        return ClassTreeNode{.selfIndex = selfIndex, .cl = cl, .relPos = ImVec2(xPos, yPos), .children = {}};
    }

    ClassTreeNode result;

    result.cl = cl;
    result.selfIndex = selfIndex;
    result.relPos.y = yPos;
    result.relPos.x = 0;

    for (int idx = 0; const auto &childName : cl->children) {
        result.children.push_back(generateTreeRec(game, game.dbClasses().getByName(childName), idx++, depth + 1));
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

auto game::menu::UpgradePanel::findParent(const Class *cl) const noexcept -> const ClassTreeNode *
{
    std::vector<const ClassTreeNode *> todo = {&m_root};

    while (!todo.empty()) {
        const auto current = todo.back();

        todo.pop_back();

        for (const auto &child : current->children) {
            if (child.cl == cl) return current;

            todo.push_back(&child);
        }
    }

    return nullptr;
}

bool game::menu::UpgradePanel::isOwned(const Class *cl) const noexcept
{
    return std::find(std::begin(m_owned), std::end(m_owned), cl) != std::end(m_owned);
}

auto game::menu::UpgradePanel::isPurchaseable(const Class *cl) const noexcept -> bool
{
    return std::find(std::begin(m_purchaseable), std::end(m_purchaseable), cl) != std::end(m_purchaseable);
}

auto game::menu::UpgradePanel::isUnavailable(const Class *cl) const noexcept -> bool
{
    return !(isOwned(cl) || isPurchaseable(cl));
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


void game::menu::UpgradePanel::event(entt::registry &world, ThePURGE &game, const engine::Event &e)
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


    if (m_spellBeingAssigned) {
        const auto spell_map = []<typename T>(T k) {
            struct SpellMap {
                int key;
                std::size_t id;
            };
            if constexpr (std::is_same<T, engine::Joystick::Buttons>::value) {
                const auto map = std::to_array<SpellMap>({{engine::Joystick::LS, 0}, {engine::Joystick::RS, 1}});
                return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

            } else if constexpr (std::is_same<T, engine::Joystick::Axis>::value) {
                const auto map = std::to_array<SpellMap>({{engine::Joystick::LST, 2}, {engine::Joystick::RST, 3}});
                return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

            } else { // todo : should be engine::Keyboard::Key
                const auto map =
                    std::to_array<SpellMap>({{GLFW_KEY_U, 0}, {GLFW_KEY_Y, 1}, {GLFW_KEY_T, 2}, {GLFW_KEY_R, 3}});
                return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;
            }
        };

        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_U:
                    case GLFW_KEY_Y:
                    case GLFW_KEY_R:
                    case GLFW_KEY_T: {
                        const auto id = spell_map(key.source.key);

                        world.get<SpellSlots>(m_player).spells[id] = game.dbSpells().instantiate(m_spellBeingAssigned->name);
                        m_spellBeingAssigned = nullptr;
                    } break;
                    default: break;
                    }
                },
                [&](const engine::Moved<engine::JoystickAxis> &joy) {
                    switch (joy.source.axis) {
                    case engine::Joystick::LST:
                    case engine::Joystick::RST: {
                        const auto id = spell_map(joy.source.axis);
                        world.get<SpellSlots>(m_player).spells[id] = game.dbSpells().instantiate(m_spellBeingAssigned->name);
                        m_spellBeingAssigned = nullptr;
                    } break;
                    default: break;
                    }
                },
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::LS:
                    case engine::Joystick::RS: {
                        const auto id = spell_map(joy.source.button);
                        world.get<SpellSlots>(m_player).spells[id] = game.dbSpells().instantiate(m_spellBeingAssigned->name);
                        m_spellBeingAssigned = nullptr;
                    } break;
                    default: return;
                    }
                },
                [&](auto) {},
            },
            e);
    }
}