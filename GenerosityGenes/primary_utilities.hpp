#pragma once
#include "worldMap.hpp"
#include "chatNeuro.hpp"
#include "include.hpp"
void render();
void renderCamera();
void worldInitialization();
sf::Color defaultColor(Types type);
namespace MinionSettings
{
    static const size_t minionInputs = 64;
    static const size_t minionOutputs = 14;
    static size_t countMiniones = 0;
}

struct Point
{
    size_t x, y;
};

class Colony;
class Minion;

enum infoMove
{
    synthesis,
    protection,
    born,
    attack,
    eat,
    move
};

class Minion
{
public:
    Minion(Point spawn_position, Colony* currentColony);
    Minion(string data);
    void nextMove();
    string SaveMe();
    void LoadMe(string data);
    Colony* myColony = nullptr;
    NeuralNetwork MyBrain;

    double hunger = 1;              //Наскільки голодний 0-помераю 1-накопичую
    double fat = 0;                 //Скільки їжи накопичено

    double points = 0;              //кількість зароблених балів
    bool IsSynthesis = false;       //Фаза синтезу
    bool IsProtection = false;      //Фаза Захисту
    bool IsDead = false;            //Чи я помер

    Point position = { 1,1 };       //Моя позиція

private:
    void LoadToWorld();

    void setMarkForMove(size_t answerId);
    double analyzeMove(infoMove move);

    std::vector<double> inputs();

    //void Attack(); //(dev tip) напиши реалізацію методу 
    infoMove move(size_t newPosX, size_t newPosY);
    void born(size_t posX, size_t posY);
    void getHungry(double count);
    void stopPhases();
    infoMove moveUp();
    infoMove moveUpLeft();
    infoMove moveUpRight();

    infoMove moveDown();
    infoMove moveDownLeft();
    infoMove moveDownRight();

    infoMove moveRight();
    infoMove moveLeft();

    infoMove StartSynthesis();
    infoMove RaiseProtection();

    infoMove bornUp();
    infoMove bornDown();
    infoMove bornLeft();
    infoMove bornRight();
    size_t id = 0;
};


class Colony
{
public:
    Colony(size_t neuronsCount, string name);
    Colony(string name);
    ~Colony();
    void createMinion(Point coordinate);
    void createMinion();
    friend class Minion;
    static void startLife();
    static void summonFruit();
    void LoadColony();
    void SaveColony();
    static void SaveMiniones(string version);
    static void LoadMiniones(string version);
    std::vector<Minion*> colonyAddresses;
    
    double coef_Synthesis;
    double coef_Defending;
    sf::Color colonyColor;
private:
    size_t _neuronsCount;
    std::string nameColony;
    NeuralNetwork* colonyBrain;
    static std::vector<Minion*> minionAddresses;
};