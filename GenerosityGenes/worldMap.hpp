#pragma once
#include "include.hpp"
class Minion;
class Colony;
class Spawner;
const size_t sizeWorldX = 440;
const size_t sizeWorldY = 240;

namespace dC //defaultColors
{
    static const sf::Color
        air = sf::Color(200,200,50,255),
        fruit = sf::Color::Red,
        border = sf::Color(50, 50, 50, 255),
        dead = sf::Color::Black;
};
struct Point
{
    size_t x, y;
    bool operator==(const Point& exmp)
    {
        return (x == exmp.x && y == exmp.y) ? true : false;
    }
    bool operator!=(const Point& exmp)
    {
        return (x == exmp.x && y == exmp.y) ? false : true;
    }
    bool inRange()
    {
        return (x < sizeWorldX&& y < sizeWorldY);
    }
};
struct Comp
{
    bool operator() (const Point& lhs, const Point& rhs) const
    {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
    }

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
    Minion* minionAddress;
};

extern std::vector<std::vector<object>> worldMap;

extern std::map<std::string, shared_ptr<Colony>> allColonies;
extern std::map<shared_ptr<Colony>, shared_ptr<Spawner>> allActiveSpawners;
extern std::set<Point,Comp> poolOfFruits;
extern std::set<Point,Comp> poolOfBorders;
extern size_t countMiniones;
extern std::map<Point, shared_ptr<Colony>, Comp> colonyArea;