#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

#include "ThePURGE.hpp"

#include "widgets/GameHUD.hpp"
#include "widgets/debug/DebugTerrainGeneration.hpp"

#include "menu/MainMenu.hpp"

auto game::ThePURGE::onCreate([[maybe_unused]] entt::registry &world) -> void
{
#ifndef NDEBUG
    m_console = std::make_unique<DebugConsole>(*this);
    m_console->info("Press TAB to autocomplete known commands.\nPress F1 to toggle this console");
#endif

    static auto holder = engine::Core::Holder{};

    m_logics = std::make_unique<GameLogic>(*this);

    m_background_music =
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/dungeon_music.wav");
    m_background_music->setVolume(0.1f).setLoop(true);

    m_db_spell.fromFile(holder.instance->settings().data_folder + "db/spells.json");
    m_db_class.fromFile(holder.instance->settings().data_folder + "db/classes.json");

    // pos and size based of `FloorGenParam::maxDungeonWidth / Height`
    EntityFactory::create<EntityFactory::ID::BACKGROUND>(*this, world, glm::vec2(25, 25), glm::vec2(75, 75));

    setMenu(std::make_unique<menu::MainMenu>());
}

auto game::ThePURGE::onDestroy(entt::registry &) -> void
{
    spdlog::info("Thank's for playing ThePURGE");

    m_logics.reset(nullptr);

    setMenu(nullptr);
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
        widget::TerrainGeneration::draw(*this, world);
    }
#endif

    if (m_currentMenu == nullptr)
        GameHUD::draw(*this, world);
    else
        m_currentMenu->onDraw(world, *this);
}
