#pragma once
#include "include.hpp"
class Minion;
class Colony;
class Spawner;
const size_t sizeWorldX = 300;
const size_t sizeWorldY = 200;
const float speedSummonFruit = 5.0f;

namespace dC //defaultColors
{
    static const sf::Color
        air = sf::Color(200,200,50,255),
        fruit = sf::Color::Red,
        border = sf::Color(50, 50, 50, 255),
        dead = sf::Color::Black;
};

enum Types
{
    minion,
    fruit,
    border,
    air,
    spawner
};


struct object
{
    Types type = Types::air;
    Minion* minionAsdress;
};

extern std::vector<std::vector<object>> worldMap;
extern std::map<std::string, Colony*> allColonys;
extern std::map<Colony*, Spawner*> allActiveSpawners;