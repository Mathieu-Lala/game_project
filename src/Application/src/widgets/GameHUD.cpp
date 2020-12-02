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
#include "widgets/Fonts.hpp"
#include "widgets/helpers.hpp"

void game::GameHUD::draw(ThePURGE &game, entt::registry &world)
{
    const auto player = game.player;

    const auto *currentClass = game.dbClasses().getByName(world.get<Classes>(player).ids.back());
    assert(currentClass != nullptr);

    const auto &health = world.get<Health>(player);
    const auto &level = world.get<Level>(player);
    const auto &skillPoints = world.get<SkillPoint>(player).count;

// #pragma region Static textures
    // clang-format off

    static GUITexture staticBackground = {
        .id =       helper::getTexture("textures/hud/hud_static.png"),
        .topleft =  helper::from1080p(25, 16),
        .size =     helper::from1080p(339, 152)
    };

    static GUITexture LB = {
        .id =       helper::getTexture("textures/hud/LB.png"),
        .topleft =  helper::from1080p(5, 160),
        .size =     helper::from1080p(30, 22)
    };

    static GUITexture LT = {
        .id =       helper::getTexture("textures/hud/LT.png"),
        .topleft =  helper::from1080p(80, 160),
        .size =     helper::from1080p(30, 27)
    };

    static GUITexture RT = {
        .id =       helper::getTexture("textures/hud/RT.png"),
        .topleft =  helper::from1080p(155, 160),
        .size =     helper::from1080p(30, 27)
    };

     static GUITexture RB = {
        .id =       helper::getTexture("textures/hud/RB.png"),
        .topleft =  helper::from1080p(230, 160),
        .size =     helper::from1080p(30, 22)
    };

    static GUITexture UpgradeIcon = {
        .id =       helper::getTexture("textures/hud/UpgradeIcon.png"),
        .topleft =  helper::from1080p(337, 125),
        .size =     helper::from1080p(26, 33)
    };
    // clang-format on
// #pragma endregion Static textures

    GUITexture portrait = {
        .id = helper::getTexture(currentClass->iconPath),
        .topleft = helper::from1080p(28, 25),
        .size = helper::from1080p(70, 80)};


    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(helper::frac2pixel({1.f, 1.f}));
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoDecoration);

    helper::drawTexture(staticBackground);

    helper::drawTexture(portrait);


    drawHealthBar(health);
    drawXpBar(level);

    ImGui::PushFont(Fonts::kimberley_23);
    {
        ImGui::SetCursorPos(helper::frac2pixel(helper::from1080p(377, 20)));
        ::helper::ImGui::Text("{}/{}", health.current, health.max);

        ImGui::SetCursorPos(helper::frac2pixel(helper::from1080p(377, 63)));
        ::helper::ImGui::Text("{}/{}", level.current_xp, level.xp_require);
    }
    ImGui::PopFont();


    const auto &spells = world.get<SpellSlots>(player).spells;

    constexpr auto spellX = std::to_array({26.0f, 251.0f, 101.0f, 176.0f});

    for (std::size_t i = 0; i < 4; ++i) {
        if (!spells[i].has_value()) continue;

        GUITexture spell{
            .id = helper::getTexture(game.dbSpells().db.at(std::string(spells[i]->id)).iconPath),
            .topleft = helper::from1080p(spellX[i], 119),
            .size = helper::from1080p(48.0f, 48.0f)};

        helper::drawTexture(spell);

        if (spells[i]->cd.is_in_cooldown)
            drawSpellCooldown(
                spellX[i],
                static_cast<float>(spells[i]->cd.remaining_cooldown.count())
                    / static_cast<float>(spells[i]->cd.cooldown.count()));
    }

    helper::drawTexture(LB);
    helper::drawTexture(LT);
    helper::drawTexture(RT);
    helper::drawTexture(RB);


    if (skillPoints) {
        helper::drawTexture(UpgradeIcon);

        ImGui::PushFont(Fonts::kimberley_23);
        ImGui::SetCursorPos(helper::frac2pixel(helper::from1080p(371, 131)));
        ::helper::ImGui::Text("{}", skillPoints);
        ImGui::PopFont();
    }

    ImGui::End();
}

void game::GameHUD::drawHealthBar(const Health &health)
{
    const auto fraction = health.current / health.max;

    ImVec2 topLeft = helper::frac2pixel(helper::from1080p(137, 16));
    ImVec2 bottomRight = helper::frac2pixel(helper::from1080p(std::lerp(137, 364, fraction), 51));

    ImVec4 color(0, 0, 0, 1);
    if (fraction < 0.5f) {
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
    const auto fraction = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);

    ImVec2 topLeft = helper::frac2pixel(helper::from1080p(137, 57));
    ImVec2 bottomRight = helper::frac2pixel(helper::from1080p(std::lerp(137, 364, fraction), 92));
    ImVec4 color(0, 0.5, 1, 1);

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(color));
}

void game::GameHUD::drawSpellCooldown(float spellX, float remaining)
{
    ImVec2 topLeft = helper::frac2pixel(helper::from1080p(spellX, std::lerp(167, 119, remaining)));
    ImVec2 bottomRight = helper::frac2pixel(helper::from1080p(spellX + 48, 167));
    ImVec4 color(.3f, .3f, .3f, .75);

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(color));
}
