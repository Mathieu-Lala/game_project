#include "ThePURGE.hpp"
#include "Engine/Core.hpp"

#include "DataConfigLoader.hpp"

#include "widgets/GameHUD.hpp"
#include "widgets/debug/DebugTerrainGeneration.hpp"

#include "menu/MainMenu.hpp"

game::ThePURGE::ThePURGE() {}

auto game::ThePURGE::onDestroy(entt::registry &) -> void {}

auto game::ThePURGE::onCreate([[maybe_unused]] entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    m_logics = std::make_unique<GameLogic>(*this);
    m_debugConsole = std::make_unique<DebugConsole>(*this);
    m_debugConsole->info("Press TAB to autocomplete known commands.\nPress F1 to toggle this console");

    m_dungeonMusic =
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/dungeon_music.wav");
    m_dungeonMusic->setVolume(0.1f).setLoop(true);


    m_classDatabase = DataConfigLoader::loadClassDatabase(holder.instance->settings().data_folder + "db/classes.json");

    // pos and size based of `FloorGenParam::maxDungeonWidth / Height`
    EntityFactory::create<EntityFactory::ID::BACKGROUND>(world, glm::vec2(25, 25), glm::vec2(75, 75));

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
        m_debugConsole->draw();
        ImGui::ShowDemoWindow();
         widget::TerrainGeneration::draw(*this, world);
    }
#endif

    if (m_currentMenu == nullptr)
        GameHUD::draw(*this, world);
    else
        m_currentMenu->onDraw(world, *this);
}
