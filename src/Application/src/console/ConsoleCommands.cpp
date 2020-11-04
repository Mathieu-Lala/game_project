#include <spdlog/spdlog.h>
#include "console/ArgParsingUtils.hpp"

#include <Engine/audio/Sound.hpp>

#include "console/ConsoleCommands.hpp"
#include "component/all.hpp"
#include "ThePURGE.hpp"

game::CommandHandler::CommandHandler() :
    m_commands{
        {"kill", cmd_kill},
        {"setSpell", cmd_setSpell},
        {"addXp", cmd_addXp},
        {"addLevel", cmd_addLevel},
        {"setMusicVolume", cmd_setMusicVolume},
        {"buyClass", cmd_buyClass},
    }
{
}

auto game::CommandHandler::getCommandHandler(const std::string &cmd) -> const game::CommandHandler::handler_t &
{
    return m_commands.at(cmd);
}

std::vector<std::string> game::CommandHandler::getCommands() const
{
    return std::accumulate(std::begin(m_commands), std::end(m_commands), std::vector<std::string>{}, [](auto init, auto &i) {
        init.emplace_back(i.first);
        return init;
    });
}

game::CommandHandler::handler_t game::CommandHandler::cmd_kill =
    [](entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto what = lexicalCast<std::string>(args[0]);

            if (what == "player") {
                for (auto &e : world.view<entt::tag<"player"_hs>>()) game.getLogics().entity_killed(world, e, e);
            } else if (what == "boss") {
                for (auto &e : world.view<entt::tag<"boss"_hs>>())
                    game.getLogics().entity_killed(world, e, game.player);

            } else
                throw std::runtime_error(fmt::format("Invalid argument {}", what));

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: kill boss|player", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_setSpell =
    [](entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 2) throw std::runtime_error("Wrong argument count");

            [[maybe_unused]] auto idx = lexicalCast<int>(args[0]);
            [[maybe_unused]] auto spell = lexicalCast<int>(args[0]);

            if (idx < 0 || idx > 4) throw std::runtime_error(fmt::format("Wrong index : {}", args[0]));

            throw std::runtime_error("Not implemented : require PR #62");

            (void) game;
            (void) world;
            // auto player = game.player;
            // auto &spellSlots = world.get<SpellSlots>(player);
            // spellSlots.spell[idx] = Spell(spell)


        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: setSpell index spell_id", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_addXp =
    [](entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto amount = lexicalCast<std::uint32_t>(args[0]);

            auto player = game.player;

            auto &level = world.get<Level>(player);

            level.current_xp += amount;
            level.current_level += static_cast<std::uint32_t>(std::floor(level.current_xp / level.xp_require));
            level.current_xp = level.current_xp % level.xp_require;

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: addXp xp", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_addLevel =
    [](entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto amount = lexicalCast<std::uint32_t>(args[0]);

            auto player = game.player;

            auto &level = world.get<Level>(player);

            level.current_level += amount;

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: addLevel level", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_setMusicVolume =
    []([[maybe_unused]] entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto volume = lexicalCast<float>(args[0]);

            game.getMusic()->setVolume(volume);

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: setMusicVolume volume\n\tvolume : [0; 2]", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_buyClass =
    []([[maybe_unused]] entt::registry &world, ThePurge &game, std::vector<std::string> &&args) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto classId = static_cast<Class::ID>(lexicalCast<int>(args[0]));
            const auto player = game.player;

            const auto &classData = game.getClassDatabase().at(classId);

            game.getLogics().onPlayerBuyClass.publish(world, player, classData);

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: buyClass id", e.what()));
        }
    };
