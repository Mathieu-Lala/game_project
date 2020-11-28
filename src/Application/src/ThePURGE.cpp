#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>
#include <Engine/Graphics/Window.hpp>

#include "ThePURGE.hpp"

#include "DataConfigLoader.hpp"

#include "widgets/GameHUD.hpp"
#include "widgets/debug/DebugTerrainGeneration.hpp"

#include "menu/MainMenu.hpp"

auto game::ThePURGE::onDestroy(entt::registry &) -> void { spdlog::info("Thanks for playing ThePURGE"); }

auto game::ThePURGE::onCreate([[maybe_unused]] entt::registry &world) -> void
{
#ifndef NDEBUG
    m_console = std::make_unique<DebugConsole>(*this);
    m_console->info("Press TAB to autocomplete known commands.\nPress F1 to toggle this console");
#endif

    static auto holder = engine::Core::Holder{};

    holder.instance->window()->setSize(glm::ivec2(1920, 1080));

    m_logics = std::make_unique<GameLogic>(*this);

    m_background_music =
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/dungeon_music.wav");
    m_background_music->setVolume(0.1f).setLoop(true);

    m_classDatabase = DataConfigLoader::loadClassDatabase(holder.instance->settings().data_folder + "db/classes.json");

    setMenu(std::make_unique<menu::MainMenu>());
}

auto game::ThePURGE::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
    if (m_currentMenu == nullptr)
        m_logics->onEvent.publish(world, e);
    else
        m_currentMenu->onEvent(world, *this, e);
}

auto game::ThePURGE::drawUserInterface(entt::registry &world) -> void
{
#ifndef NDEBUG
    static auto holder = engine::Core::Holder{};

    if (holder.instance->isShowingDebugInfo()) {
        m_console->draw();
        ImGui::ShowDemoWindow();
        widget::TerrainGeneration::draw(*this, world);
    }
#endif

    if (m_currentMenu == nullptr)
        GameHUD::draw(*this, world);
    else
        m_currentMenu->onDraw(world, *this);
}
