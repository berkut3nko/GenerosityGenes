#pragma once
#include "worldMap.hpp"
#include "chatNeuro.hpp"
#include "include.hpp"
void render();
extern bool isMainWindowOpen;
void InitializationRender();
void worldInitialization();
sf::Color defaultColor(Types type);
extern float multiplicator;

extern bool isStoped;

namespace MinionSettings
{
    static const size_t minionInputs = 77;
    static const size_t minionOutputs = 14;
    static size_t countMiniones = 0;
    static float eat_cost = 1.0f;
}



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
    eatTeammate,
    eatEnemy,
    move,
    moveToBorder
};

class Minion
{
public:
    Minion(Point spawn_position, Colony* currentColony);
    Minion(Point spawn_position, Colony* currentColony, NeuralNetwork* parentBrain, double hungerFromParent);
    Minion(Colony* _myColony, Point _position, double _fat, double _hunger);

    void nextMove();
    string SaveMe();
    void LoadMe(string data);
    Colony* myColony = nullptr;
    NeuralNetwork MyBrain;

    double hunger = 0.3;              //�������� �������� 0-������� 1-���������
    double fat = .0;                  //������ ��� ����������
    double health = 1.0;

    double points = 0;              //������� ���������� ����
    bool IsSynthesis = false;       //���� �������
    bool IsProtection = false;      //���� �������
    bool IsDead = false;            //�� � �����

    Point position = { 1,1 };       //��� �������

    vector<double> memmory;
    void kill();
private:
    size_t rotting = 0;

    void LoadToWorld();

    void setMarkForMove(size_t answerId);
    double analyzeMove(infoMove move);
    double analyzePos(infoMove move);

    std::vector<double> inputs();

    void Attack(Minion* minion);
    infoMove interact(size_t newPosX, size_t newPosY);
    void move(size_t MovePosX, size_t MovePosY);
    void allocateArea();
    infoMove born(size_t posX, size_t posY);
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
    Colony(size_t neuronsCountFirst, size_t neuronsCountSecond, std::string name);
    Colony(string name);
    ~Colony();
    void coefInitialization();
    void coefInitialization(ColonolyCoefPreset preset);
    void createMinion(Point coordinate);
    void createMinion();
    void createMinion(Point coordinate, Minion* parent, double hunger);
    friend class Minion;
    static void startLife();
    static void summonFruit();
    static void SaveColonies(string version);
    static void LoadColonies(string version);
    void LoadColony();
    void SaveColony();
    static void SaveMiniones(string version);
    static void LoadMiniones(string version);
    static std::vector<Minion*> minionAddresses;
    static double AVRGpoints;
    std::vector<Minion*> colonyAddresses;
    std::map<Colony*, float> colonyRelations;
    
    size_t sizeColony = 0;
    size_t sizeMemmory = 2;
    float coef_Synthesis;
    float coef_Protection;
    float coef_Born;
    float coef_AttackEnemy;
    float coef_Eat;
    float coef_AttackTeam;
    float coef_Border;
    float coef_SpawnerEnemy;
    float coef_SpawnerTeam;

    float coef_TeamClose;
    float coef_EnemyClose;
    float coef_TeamSpawnerClose;
    float coef_EnemySpawnerClose;
    float coef_EatClose;

    bool hasSpawner = false;
    size_t colonyMinSize = 0;

    sf::Color colonyColor;
private:
    std::pair<size_t,size_t> _neuronsCount;
    std::string nameColony;
    NeuralNetwork bestMinionBrain;
    //NeuralNetwork colonyBrain;
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