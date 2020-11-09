#include "models/ClassDatabase.hpp"

auto game::classes::getByName(const Database &db, const std::string &name) -> std::optional<const Class *>
{
    for (const auto &[_, classData] : db)
        if (classData.name == name) return &classData;
    return {};
}