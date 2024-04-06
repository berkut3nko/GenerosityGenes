#include "primary_utilities.hpp"
using namespace MinionSettings;


Minion::Minion(Point spawn_position, shared_ptr<Colony> currentColony) :position(spawn_position), myColony(currentColony),
MyBrain({ {MinionSettings::minionInputs + myColony->sizeMemmory, myColony->_neuronsCount.first},
          {myColony->_neuronsCount.first, myColony->_neuronsCount.second},
          {myColony->_neuronsCount.second,MinionSettings::minionOutputs + myColony->sizeMemmory} }, myColony->nameColony)
{
    ID = myColony->nextMinionID;
    ++myColony->nextMinionID;

    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;
    ++countMiniones;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
}
Minion::Minion(Point spawn_position, shared_ptr<Colony> currentColony, NeuralNetwork* parentBrain, double hungerFromParent) :position(spawn_position), myColony(currentColony), MyBrain(*parentBrain), hunger(hungerFromParent)
{
    ID = Colony::nextMinionID;
    ++myColony->nextMinionID;

    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;
    ++countMiniones;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
}
Minion::Minion(shared_ptr<Colony> _myColony, Point _position, double _fat, double _hunger) :myColony(_myColony), MyBrain(_myColony->bestMinionBrain), position(_position), fat(_fat), hunger(_hunger)
{

    ID = Colony::nextMinionID;
    ++myColony->nextMinionID;

    ++countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;
    MyBrain = myColony->bestMinionBrain;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
}
Minion::Minion(const Minion& other) :
    myColony(other.myColony),
    MyBrain(myColony->bestMinionBrain),
    position(other.position),
    fat(other.fat),
    hunger(other.hunger),
    health(other.health)
{
    ID = Colony::nextMinionID;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    // Копіюємо всі поля класу Minion
}

Minion& Minion::operator=(const Minion& other) {
    if (this != &other) { // Перевірка на самокопіювання
        // Копіюємо всі поля класу Minion
        this->myColony = other.myColony;
        this->MyBrain = this->myColony->bestMinionBrain;
        this->hunger = other.hunger;
        this->fat = other.fat;
        this->health = other.health;
        this->position = other.position;
    }
    ID = Colony::nextMinionID;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
    return *this;
}
