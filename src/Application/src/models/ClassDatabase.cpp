#include <Engine/helpers/macro.hpp>
#include "models/ClassDatabase.hpp"

auto game::classes::getByName(const Database &db, const std::string_view name) -> const Class *
{
    if (const auto found = std::find_if(db.begin(), db.end(), [&name](const auto &i) { return i.second.name == name; });
        found != db.end()) {
        return &found->second;
    } else
        UNLIKELY { return nullptr; }
}
