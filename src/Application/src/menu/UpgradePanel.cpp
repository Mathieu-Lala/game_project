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
    const auto ownedClasses = world.get<Classes>(player).ids;
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


    const bool isClassOwned =
        std::find(std::begin(ownedClasses), std::end(ownedClasses), m_selectedClass->name) != std::end(ownedClasses);

    if (isClassOwned) {
        helper::drawTexture(btn_alreadyowned);
    } else {
        const bool isClassBuyable = true; // TODO: with tree

        if (isClassBuyable && skillPoints >= m_selectedClass->cost)
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
    /*

    static bool choosetrigger = false;
    static auto test = game.dbClasses().getStarterClass();
    static std::optional<Class> selectedClass;
    static std::string chosenTrig = "";
    static int spellmapped = 0;
    static int infoAdd;

    static auto texture = [data = holder.instance->settings().data_folder]() {
        // clang-format off
            const auto path = std::to_array({
                data + "/textures/InfoHud.png",
                data + "/textures/CPoint.PNG",
                data + "/textures/validate.png",
                data + "/textures/controller/LB.png",
                data + "/textures/controller/LT.png",
                data + "/textures/controller/RT.png",
                data + "/textures/controller/RB.png"
            });
        // clang-format on
        std::array<std::uint32_t, path.size()> out;
        std::generate(out.begin(), out.end(), [&path, n = 0ul]() mutable {
            const auto &o = engine::Core::Holder{}.instance->getCache<engine::Texture>().load<engine::LoaderTexture>(
                entt::hashed_string{fmt::format("resource/texture/identifier/{}", path.at(n)).data()}, path.at(n));
            n++;
            return o->id;
        });
        return out;
    }();

    ImVec2 size = ImVec2(1000.0f, 1000.0f);
    ImGui::SetNextWindowPos(ImVec2(game.getCamera().getCenter().x + size.x / 2, 0));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
    ImGui::Begin(
        "Evolution Panel",
        nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
    size = ImGui::GetWindowSize();
    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((texture[0]))), ImVec2(size.x - 30, size.y - 10));
    ImGui::SetCursorPos(ImVec2(0, 200));
    ImVec2 next;
    if (selectedClass.has_value()) {
        ImVec2 icon = ImVec2(ImGui::GetCursorPosX() + size.x / 3 + 9, ImGui::GetCursorPosY());
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3 + 9, ImGui::GetCursorPosY()));
        helper::ImGui::Text("Class Name: {}", selectedClass->name);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
        helper::ImGui::Text("Class description: {}", selectedClass->description);
        if (infoAdd == 1) {
            static std::size_t triggerValue = 5;
            std::optional<Classes> lastclass = world.get<Classes>(player);
            if (spellmapped % 2 != 0 && lastclass.value().ids.size() > 1
                && lastclass.value().ids[lastclass.value().ids.size() - 1] == selectedClass.value().name) {
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 50));
                ImGui::Text("Choose Your trigger :");
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                auto &spell = world.get<SpellSlots>(player);
                if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((texture[3]))), ImVec2(50, 50))) {
                    chosenTrig = "LB";
                    triggerValue = 0;
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((texture[4]))), ImVec2(50, 50))) {
                    chosenTrig = "LT";
                    triggerValue = 2;
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((texture[5]))), ImVec2(50, 50))) {
                    chosenTrig = "RT";
                    triggerValue = 3;
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((texture[6]))), ImVec2(50, 50))) {
                    chosenTrig = "RB";
                    triggerValue = 1;
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                helper::ImGui::Text("You choosed the trigger: {}", chosenTrig.c_str());
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                if (chosenTrig != "") {
                    if (ImGui::Button("Validate")) {
                        choosetrigger = false;
                        auto NewSpell = game.dbSpells().instantiate(selectedClass.value().spells[0]).value();
                        for (auto i = 0ul; i < spell.spells.size(); i++) {
                            if (spell.spells[i].has_value() && NewSpell.id == spell.spells[i].value().id) {
                                spell.spells[i] = {};
                            }
                        }
                        spell.spells[triggerValue] = NewSpell;
                        spellmapped++;
                    }
                }
            } else {
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                ImGui::Text("Already bought");
            }
            next = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
            ImGui::SetCursorPos(ImVec2(icon.x - 50, icon.y));
            ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((texture[2]))), ImVec2(50, 50));
        } else if (infoAdd == 2) {
            if (skillPoints >= selectedClass.value().cost) {
                if (choosetrigger == true) {
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                    ImGui::Text("You haven't choose a trigger for the spell");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                if (ImGui::Button("Add class")) {
                    game.logics()->onPlayerPurchase.publish(world, player, selectedClass.value());
                    infoAdd = 1;
                    if (choosetrigger == false) {
                        spellmapped++;
                        choosetrigger = true;
                    } else
                        spellmapped = spellmapped + 2;
                }
            } else {
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                helper::ImGui::Button("No skill point", ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
            }

        } else {
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
            ImGui::Text("You can't buy it yet");
        }
    }
    // Competences
    ImGui::SetCursorPos(ImVec2(size.x / 2 - 17 * ImGui::GetFontSize() / 2, size.y / 2));
    helper::ImGui::Text("Point de comp: {}", skillPoints); // rendre dynamique le nombre de point de comp
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 5));
    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((texture[1]))), ImVec2(20, 20));

    // Classes tree
    float length = size.x / 2 - (12.0f + static_cast<float>(test.name.length())) * ImGui::GetFontSize();
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + length, ImGui::GetCursorPosY() + 30));
    helper::ImGui::Text("class Name: {}", test.name);

    std::vector<std::string> classes = {game.dbClasses().getStarterClass().name};
    std::vector<std::string> purchasable;

    while (!classes.empty()) {
        std::vector<std::string> children;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 4, ImGui::GetCursorPosY() + 10));
        for (const auto &id : classes) {
            const auto &data = *game.dbClasses().getByName(id);

            if (std::find(already_purchased.begin(), already_purchased.end(), id) != already_purchased.end()) {
                purchasable.insert(purchasable.end(), data.children.begin(), data.children.end());
                if (helper::ImGui::Button(data.name, ImVec4(0, 1, 0, 0.5))) {
                    selectedClass = data;
                    infoAdd = 1;
                }
            } else if (std::find(purchasable.begin(), purchasable.end(), id) != purchasable.end()) {
                if (helper::ImGui::Button(data.name, ImVec4(1, 1, 0, 0.5))) {
                    selectedClass = data;
                    infoAdd = 2;
                }
            } else {
                if (helper::ImGui::Button(data.name, ImVec4(1, 0, 0, 0.5))) {
                    selectedClass = data;
                    infoAdd = 3;
                }
            }
            ImGui::SameLine();
            children.insert(children.end(), data.children.begin(), data.children.end());
        }
        classes = std::move(children);
        ImGui::Text(" "); // ImGui::NextLine()
    }
    ImGui::End();
    */
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




void game::menu::UpgradePanel::printClassTreeDebug() const noexcept
{
    spdlog::info("================ CLASS TREE DEBUG ================");
    spdlog::info("Tree :");

    for (int i = 0; const auto &tier : m_classes) {
        spdlog::info(" Tier {} :", ++i);

        for (const auto *cl : tier)
            spdlog::info("\t{}", cl->name);
    }

    spdlog::info("");
    spdlog::info("Owned :");
    for (const auto *owned : m_owned)
        spdlog::info("\t{}", owned->name);

    spdlog::info("");
    spdlog::info("Purchaseable :");
    for (const auto *purchaseable : m_purchaseable)
        spdlog::info("\t{}", purchaseable->name);


    spdlog::info("==================================================");
}