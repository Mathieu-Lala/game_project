#include <cassert>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>
#include <Engine/helpers/ImGui.hpp>

#include "ThePURGE.hpp"

#include "models/Spell.hpp"
#include "models/Class.hpp"

#include "component/all.hpp"

#include "widgets/GameHUD.hpp"
#include "widgets/helpers.hpp"

void game::GameHUD::draw(ThePURGE &game, entt::registry &world)
{
    static auto holder = engine::Core::Holder{};

    const auto player = game.player;

    const auto *currentClass = game.dbClasses().getByName(world.get<Classes>(player).ids.back());
    assert(currentClass != nullptr);

    const auto &health = world.get<Health>(player);

#pragma region Textures
    // clang-format off


    GUITexture staticBackground = {
        .id =       helper::getTexture("textures/hud/hud_static.png"),
        .topleft =  helper::from1080p(25, 16),
        .size =     helper::from1080p(339, 152)
    };

    GUITexture portrait = {
        .id =       helper::getTexture(currentClass->iconPath),
        .topleft =  helper::from1080p(28, 25),
        .size =     helper::from1080p(70, 80)
    };

    GUITexture LB = {
        .id =       helper::getTexture("textures/hud/LB.png"),
        .topleft =  helper::from1080p(5, 160),
        .size =     helper::from1080p(30, 22)
    };

    GUITexture LT = {
        .id =       helper::getTexture("textures/hud/LT.png"),
        .topleft =  helper::from1080p(80, 160),
        .size =     helper::from1080p(30, 27)
    };

    GUITexture RT = {
        .id =       helper::getTexture("textures/hud/RT.png"),
        .topleft =  helper::from1080p(155, 160),
        .size =     helper::from1080p(30, 27)
    };

     GUITexture RB = {
        .id =       helper::getTexture("textures/hud/RB.png"),
        .topleft =  helper::from1080p(230, 160),
        .size =     helper::from1080p(30, 22)
    };
    // clang-format on
#pragma endregion Textures


    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(helper::frac2pixel({1.f, 1.f}));
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoDecoration);

    helper::drawTexture(staticBackground);
    helper::drawTexture(portrait);

    drawHealthBar(health);
    drawXpBar(world.get<Level>(player));


    const auto &spells = world.get<SpellSlots>(player).spells;

    std::array<float, 4> spellX = {26, 101, 176, 251};

    for (int i = 0; i < 4; ++i) {
        if (!spells[i].has_value()) continue;

        GUITexture spell{
            .id = helper::getTexture(game.dbSpells().db.at(std::string(spells[i]->id)).iconPath),
            .topleft = helper::from1080p(spellX[i], 119),
            .size = helper::from1080p(48, 48)};

        helper::drawTexture(spell);

        // TODO: cooldown
    }

    helper::drawTexture(LB);
    helper::drawTexture(LT);
    helper::drawTexture(RT);
    helper::drawTexture(RB);

    ImGui::End();

    // --------------------------------------------------------------------------------------------------------------

    // auto holder = engine::Core::Holder{};

    //// note : don t need to hold a Gluint as it exist in cache
    // static GLuint texture =
    //    holder.instance->getCache<engine::Texture>()
    //        .load<engine::LoaderTexture>(
    //            entt::hashed_string{
    //                fmt::format("resource/texture/identifier/{}", holder.instance->settings().data_folder + "/textures/InfoHud.png")
    //                    .data()},
    //            holder.instance->settings().data_folder + "/textures/InfoHud.png")
    //        ->id;

    // const auto infoHealth = world.get<Health>(game.player);
    // const auto HP = infoHealth.current / infoHealth.max;
    // const auto level = world.get<Level>(game.player);
    // const auto XP = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);

    // ImGui::Begin(
    //    "Info Player",
    //    nullptr,
    //    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
    //        | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
    // ImVec2 size = ImGui::GetWindowSize();
    // ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((texture))), ImVec2(size.x - 30, size.y - 10));
    // ImGui::SetCursorPos(ImVec2(ImGui::GetItemRectMin().x + 40, ImGui::GetItemRectMin().y + size.y / 7));
    // ImGui::ProgressBar(HP, ImVec2(0.f, 0.f), fmt::format("{}/{}", infoHealth.current, infoHealth.max).data());
    // ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    // ImGui::Text("HP");
    // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    // ImGui::ProgressBar(XP, ImVec2(0.0f, 0.0f));
    // ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    // ImGui::Text("XP");
    // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    // helper::ImGui::Text("LEVEL: {} ~~~ SKILL POINTS: {}", level.current_level, world.get<SkillPoint>(game.player).count);
    //// ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    //// ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    //// helper::ImGui::Text("Speed: {}", 1);
    //// ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    //// helper::ImGui::Text("Atk: {}", Atk.damage);

    // for (const auto &i : world.get<SpellSlots>(game.player).spells) {
    //    if (i.has_value()) {
    //        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    //        ImGui::ProgressBar(
    //            static_cast<float>(i.value().cd.remaining_cooldown.count())
    //                / static_cast<float>(i.value().cd.cooldown.count()),
    //            ImVec2(0.f, 0.f),
    //            fmt::format("{}/{}", i.value().cd.remaining_cooldown.count(), i.value().cd.cooldown.count()).data());
    //    }
    //}
    // if (world.get<KeyPicker>(game.player).hasKey) {
    //    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
    //    helper::ImGui::Text("You have the key");
    //}

    // ImGui::End();
}

void game::GameHUD::drawHealthBar(const Health &health)
{
    float fraction = health.current / health.max;

    ImVec2 topLeft = helper::frac2pixel(helper::from1080p(137, 16));
    ImVec2 bottomRight = helper::frac2pixel(helper::from1080p(std::lerp(137, 364, fraction), 51));

    ImVec4 color(0, 0, 0, 1);
    if (fraction < 0.5) {
        // red
        color.x = 1;
        // green
        color.y = std::lerp(0, 1, fraction * 2);
    } else {
        // red
        color.x = std::lerp(1, 0, (fraction - 0.5f) * 2);
        // green
        color.y = 1;
    }

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(color));
}

void game::GameHUD::drawXpBar(const Level &level)
{
    float fraction = static_cast<float>(level.current_xp) / level.xp_require;

    ImVec2 topLeft = helper::frac2pixel(helper::from1080p(137, 57));
    ImVec2 bottomRight = helper::frac2pixel(helper::from1080p(std::lerp(137, 364, fraction), 92));
    ImVec4 color(0, 0.5, 1, 1);

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(color));
}
