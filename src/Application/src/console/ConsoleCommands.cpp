#include "console/consoleCommands.hpp"
#include <spdlog/spdlog.h>
#include "console/ArgParsingUtils.hpp"

#include <Engine/audio/Sound.hpp>

#include "component/all.hpp"
#include "ThePURGE.hpp"

game::CommandHandler::CommandHandler()
{
    m_commands["kill"] = cmd_kill;
    m_commands["setSpell"] = cmd_setSpell;
    m_commands["addXp"] = cmd_addXp;
    m_commands["addLevel"] = cmd_addLevel;
    m_commands["setMusicVolume"] = cmd_setMusicVolume;
}

auto game::CommandHandler::getCommandHandler(const std::string &cmd) -> const game::CommandHandler::handler_t &
{
    return m_commands.at(cmd);
}

std::vector<std::string> game::CommandHandler::getCommands() const
{
    std::vector<std::string> result;

    result.reserve(m_commands.size());

    for (const auto &[k, v] : m_commands) result.push_back(k);

    return result;
}

void game::CommandHandler::cmd_kill(entt::registry &world, ThePurge &game, std::vector<std::string> &&args)
{
    std::string what;

    try {
        if (args.size() != 1) throw std::runtime_error("Wrong argument count");

        what = lexicalCast<std::string>(args[0]);

        if (what == "player") {
            for (auto &e : world.view<entt::tag<"player"_hs>>()) game.getLogics().entity_killed(world, e, e);
        } else if (what == "boss") {
            for (auto &e : world.view<entt::tag<"boss"_hs>>()) game.getLogics().entity_killed(world, e, game.player);

        } else
            throw std::runtime_error(fmt::format("Invalid argument {}", what));

    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: kill boss|player", e.what()));
    }
}


void game::CommandHandler::cmd_setSpell(entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
    int idx, spell;

    try {
        if (args.size() != 2) throw std::runtime_error("Wrong argument count");

        idx = lexicalCast<int>(args[0]);
        spell = lexicalCast<int>(args[0]);

        if (idx < 0 || idx > 4)
            throw std::runtime_error(fmt::format("Wrong index : {}", args[0]));

        throw std::runtime_error("Not implemented : require PR #62");

        (void) game;
        (void) world;
        //auto player = game.player;
        //auto &spellSlots = world.get<SpellSlots>(player);
        // spellSlots.spell[idx] = Spell(spell)


    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: setSpell index spell_id", e.what()));
    }
}


void game::CommandHandler::cmd_addXp(entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
    int amount;

    try {
        if (args.size() != 1) throw std::runtime_error("Wrong argument count");

        amount = lexicalCast<int>(args[0]);

        auto player = game.player;

        auto &level = world.get<Level>(player);

        level.current_xp += amount;
        level.current_level += static_cast<std::uint32_t>(std::floor(level.current_xp / level.xp_require));
        level.current_xp = level.current_xp % level.xp_require;

    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: addXp xp", e.what()));
    }
}
void game::CommandHandler::cmd_addLevel(entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
    int amount;

    try {
        if (args.size() != 1) throw std::runtime_error("Wrong argument count");

        amount = lexicalCast<int>(args[0]);

        auto player = game.player;

        auto &level = world.get<Level>(player);

        level.current_level += amount;

    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: addLevel level", e.what()));
    }
}

void game::CommandHandler::cmd_setMusicVolume(entt::registry &, ThePurge &game, std::vector<std::string> &&args)
{
    float volume;

    try {
        if (args.size()!=  1) throw std::runtime_error("Wrong argument count");

        volume = lexicalCast<float>(args[0]);

        game.getMusic()->setVolume(volume);

    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: setMusicVolume volume\n\tvolume : [0; 2]", e.what()));
    }

}
