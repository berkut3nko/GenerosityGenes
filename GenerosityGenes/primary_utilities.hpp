#pragma once
#include "worldMap.hpp"
#include "chatNeuro.hpp"
#include "include.hpp"
void render();
extern bool isMainWindowOpen;
void InitializationRender();
void worldInitialization();
sf::Color defaultColor(Types type);
namespace MinionSettings
{
    static const size_t minionInputs = 200;
    static const size_t minionOutputs = 14;
    static size_t countMiniones = 0;
}

struct Point
{
    size_t x, y;
};

class Colony;
class Minion;
class Spawner;

enum infoMove
{
    synthesis,
    protection,
    born,
    attackEnemy,
    attackTeam,
    eat,
    move
};

class Minion
{
public:
    Minion(Point spawn_position, Colony* currentColony);
    Minion(Point spawn_position, Colony* currentColony, NeuralNetwork* parentBrain, double hungerFromParent);
    Minion(string data);

    void nextMove();
    string SaveMe();
    void LoadMe(string data);
    Colony* myColony = nullptr;
    NeuralNetwork MyBrain;

    double hunger = 0.3f;              //�������� �������� 0-������� 1-���������
    double fat = 0;                 //������ ��� ����������

    double points = 0;              //������� ���������� ����
    bool IsSynthesis = false;       //���� �������
    bool IsProtection = false;      //���� �������
    bool IsDead = false;            //�� � �����

    Point position = { 1,1 };       //��� �������

    vector<double> memmory;

private:
    size_t rotting = 0;
    void LoadToWorld();

    void setMarkForMove(size_t answerId);
    double analyzeMove(infoMove move);
    double analyzePos(infoMove move);

    std::vector<double> inputs();

    //void Attack(); //(dev tip) ������ ��������� ������ 
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
enum ColonolyCoefPreset
{

};

class Colony
{
public:
    Colony(size_t neuronsCount, string name);
    Colony(string name);
    void coefInitialization();
    void coefInitialization(ColonolyCoefPreset preset);
    void createMinion(Point coordinate);
    void createMinion();
    void createMinion(Point coordinate, Minion* parent, double hunger);
    friend class Minion;
    static void startLife();
    static void summonFruit();
    void LoadColony();
    void SaveColony();
    static void SaveMiniones(string version);
    static void LoadMiniones(string version);
    std::vector<Minion*> colonyAddresses;
    size_t sizeColony = 0;
    size_t sizeMemmory = 2;
    double coef_Synthesis;
    double coef_Protection;
    double coef_Born;
    double coef_AttackEnemy;
    double coef_Eat;
    double coef_AttackTeam;
    double coef_Border;
    double coef_SpawnerEnemy;
    double coef_SpawnerTeam;

    sf::Color colonyColor;
private:
    size_t _neuronsCount;
    std::string nameColony;
    NeuralNetwork* colonyBrain;
    static std::vector<Minion*> minionAddresses;
};

class Spawner
{
public:
    Spawner(Colony* colony, size_t minPopulation, Point position);
    Spawner(Colony* colony, size_t minPopulation);

    Point spawnerPosition;
    Colony* summonSample = nullptr;
private:
    friend class Colony;
    Point generateCord();
    size_t populationSize;
};