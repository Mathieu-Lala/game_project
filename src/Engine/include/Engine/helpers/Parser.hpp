#pragma once

#include <sstream>
#include <string_view>
#include <vector>

#include <fmt/format.h>

// #define TYPE2STR(T) #T

template<typename T>
T lexicalCast(const std::string &str)
{
    T var;
    std::istringstream iss;
    iss.str(str);
    iss >> var;

    if (iss.fail()) throw std::runtime_error(fmt::format("Invalid argument : {}", str));

    return var;
}
