#include "primary_utilities.hpp"

Spawner::Spawner(shared_ptr<Colony> colony, size_t minPopulation, Point position) :summonSample(colony), spawnerPosition(position)
{
    if (colony->hasSpawner == false) {
        worldMap[spawnerPosition.x][spawnerPosition.y].type = Types::spawner;
        colony->hasSpawner = true;
        colony->colonyMinSize = minPopulation;
    }
    return;

}
Spawner::Spawner(shared_ptr<Colony> colony, size_t minPopulation) :summonSample(colony)
{
    if (colony->hasSpawner == false) {
        size_t Xtemp, Ytemp;
        while (true)//search place to init
        {
            Xtemp = rand() % sizeWorldX;
            Ytemp = rand() % sizeWorldY;
            if (worldMap[Xtemp][Ytemp].type == Types::air)
            {
                spawnerPosition = { Xtemp ,Ytemp };
                break;
            }
        }
        worldMap[spawnerPosition.x][spawnerPosition.y].type = Types::spawner;
        colony->hasSpawner = true;
        colony->colonyMinSize = minPopulation;

    }
    return;
}
Point Spawner::generateCord()
{
    //(dev tip) потім переписати коли зявиться карта колоній спавнити нового на території колонії
    return { (rand() % 2 == 0) ? spawnerPosition.x - 1 : spawnerPosition.x + 1,(rand() % 2 == 0) ? spawnerPosition.y - 1 : spawnerPosition.y + 1 };
}