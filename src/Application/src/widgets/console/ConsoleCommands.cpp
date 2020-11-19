#include <spdlog/spdlog.h>
#include <Engine/audio/Sound.hpp>
#include <Engine/helpers/Parser.hpp>

#include "widgets/console/ConsoleCommands.hpp"
#include "widgets/console/DebugConsole.hpp"

#include "component/all.hpp"
#include "screen/MainMenu.hpp"
#include "ThePURGE.hpp"

game::CommandHandler::CommandHandler() :
    m_commands{
        {"kill", cmd_kill},
        {"setSpell", cmd_setSpell},
        {"addXp", cmd_addXp},
        {"addLevel", cmd_addLevel},
        {"setMusicVolume", cmd_setMusicVolume},
        {"buyClass", cmd_buyClass},
        {"getClasses", cmd_getClasses},
        {"getClassInfo", cmd_getClassInfo},
        {"giantfireball", cmd_giantfireball},
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
    [](entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto what = lexicalCast<std::string>(args[0]);

            if (what == "player") {
                for (auto &e : world.view<entt::tag<"player"_hs>>())
                    game.getLogics().onEntityKilled.publish(world, e, e);
            } else if (what == "boss") {
                for (auto &e : world.view<entt::tag<"boss"_hs>>())
                    game.getLogics().onEntityKilled.publish(world, e, game.player);

            } else
                throw std::runtime_error(fmt::format("Invalid argument {}", what));

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: kill boss|player", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_setSpell =
    [](entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 2) throw std::runtime_error("Wrong argument count");

            auto idx = lexicalCast<int>(args[0]);
            auto spellId = lexicalCast<int>(args[1]);

            if (idx < 0 || idx > 4) throw std::runtime_error(fmt::format("Wrong index : {}", args[0]));

            auto player = game.player;
            auto &spellSlots = world.get<SpellSlots>(player);
            spellSlots.spells[static_cast<std::size_t>(idx)] = Spell::create(static_cast<SpellFactory::ID>(spellId));

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: setSpell index spell_id", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_addXp =
    [](entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto amount = lexicalCast<std::uint32_t>(args[0]);

            auto player = game.player;

            game.getLogics().addXp(world, player, amount);

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: addXp xp", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_addLevel =
    [](entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            auto amount = lexicalCast<std::uint32_t>(args[0]);

            auto player = game.player;

            auto &level = world.get<Level>(player);

            while (amount--) game.getLogics().addXp(world, player, level.xp_require);

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: addLevel level", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_setMusicVolume =
    []([[maybe_unused]] entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto volume = lexicalCast<float>(args[0]);

            game.getMusic()->setVolume(volume);

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: setMusicVolume volume\n\tvolume : [0; 2]", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_buyClass =
    []([[maybe_unused]] entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto className = lexicalCast<std::string>(args[0]);
            const auto player = game.player;

            if (const auto data = classes::getByName(game.getClassDatabase(), className); data) {
                game.getLogics().onPlayerBuyClass.publish(world, player, *data);
            } else {
                // note : see std::accumulate
                std::stringstream names;
                for (const auto &[_, i] : game.getClassDatabase()) { names << i.name << ", "; }
                throw std::runtime_error(fmt::format("Available classes : [ {}]", names.str()));
            }

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: buyClass name", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_getClasses =
    []([[maybe_unused]] entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &console) {
        if (args.size() != 0) throw std::runtime_error("Wrong argument count");

        const auto &classes = world.get<Classes>(game.player).ids;

        std::stringstream names;

        for (auto &id : classes) names << game.getClassDatabase().at(id).name << ", ";

        console.info("Player has {} classes : {}", classes.size(), names.str());
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_getClassInfo =
    []([[maybe_unused]] entt::registry &world, ThePURGE &game, std::vector<std::string> &&args, DebugConsole &console) {
        try {
            if (args.size() != 1) throw std::runtime_error("Wrong argument count");

            const auto className = lexicalCast<std::string>(args[0]);

            if (const auto data = classes::getByName(game.getClassDatabase(), className); !data) {
                // note : see std::accumulate
                std::stringstream names;
                for (const auto &[_, i] : game.getClassDatabase()) names << i.name << ", ";
                throw std::runtime_error(fmt::format("Available classes : [ {}]", names.str()));
            } else {
                std::stringstream spellNames;
                for (const auto &id : data->spells) spellNames << id << ", ";

                std::stringstream childrenesNames;
                for (const auto &id : data->children) childrenesNames << game.getClassDatabase().at(id).name << ", ";

                console.info(
                    "Class {} :\n"
                    "\tid : {}\n"
                    "\tdescription : {}\n"
                    "\ticon : {}\n"
                    "\tgraph asset : {}\n"
                    "\tspells : {}\n"
                    "\tmax health : {}\n"
                    "\tdamage : {}\n"
                    "\tchildren classes : {}",
                    data->name,
                    data->id,
                    data->description,
                    data->iconPath,
                    data->assetGraphPath,
                    spellNames.str(),
                    data->maxHealth,
                    data->damage,
                    childrenesNames.str());
            }

        } catch (const std::runtime_error &e) {
            throw std::runtime_error(fmt::format("{}\nusage: getClassInfo name", e.what()));
        }
    };

game::CommandHandler::handler_t game::CommandHandler::cmd_giantfireball =
    []([[maybe_unused]] entt::registry &world, ThePURGE &game, std::vector<std::string> &&, DebugConsole &) {
        SpellFactory::create(SpellFactory::DEBUG_GIANT_FIREBALL, world, game.player, glm::vec2(0, 0));
    };