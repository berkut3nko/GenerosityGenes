#pragma once
#include "include.hpp"
class Minion;
class Colony;
const size_t sizeWorldX = 30;
const size_t sizeWorldY = 30;
const float speedSummonFruit = 2.0f;

namespace dC //defaultColors
{
    static const sf::Color
        air = sf::Color::White,
        fruit = sf::Color::Red,
        border = sf::Color::Black,
        dead = sf::Color::Black;
};

enum Types
{
    minion,
    fruit,
    border,
    air
};


struct object
{
    Types type = Types::air;
    sf::Color color = dC::air;
    Minion* minionAsdress;
};

extern std::vector<std::vector<object>> worldMap;
extern std::map<std::string, Colony*> allColonys;