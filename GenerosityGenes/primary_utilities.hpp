#pragma once
#include "worldMap.hpp"
#include "chatNeuro.hpp"

#include "Client.hpp"
#include "Server.hpp"

extern string version;

struct MinionStackTexture
{
    sf::Texture idle,
                synthesis,
                protection,
                dead;

};
extern vector<std::pair<MinionStackTexture, bool>> minionTexturesPool; //MinionStackTexture and if his claimed 

void loadTextures();



void render();

extern bool isMainWindowOpen;
void InitializationRender();
void worldInitialization();
sf::Color defaultColor(Types type);
extern float multiplicator;

extern bool isStoped;
static float speedSummonFruit = 5.0f;

//Connection
extern sf::IpAddress serverIpAdress;
extern unsigned short serverPort;
extern char userName[64];
extern NetworkClient netConnection;
extern vector<string> namesVec;
extern bool isConnected;
//Server
extern bool isServerOpened;
extern NetworkServer netServer;
void openServer();
extern Time lastPacketSend;

namespace MinionSettings
{
    static const size_t minionInputs = 77;
    static const size_t minionOutputs = 14;
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
    Minion(Point spawn_position, shared_ptr<Colony> currentColony);
    Minion(Point spawn_position, shared_ptr<Colony> currentColony, NeuralNetwork* parentBrain, double hungerFromParent);
    Minion(shared_ptr<Colony> _myColony, Point _position, double _fat, double _hunger);
    Minion(const Minion& other);
    Minion& operator=(const Minion& other);
    size_t ID;
    void nextMove();
    string SaveMe();
    void LoadMe(string data);
    shared_ptr<Colony> myColony = nullptr;
    NeuralNetwork MyBrain;

    double hunger = 0.3;              //Наскільки голодний 0-помераю 1-накопичую
    double fat = .0;                  //Скільки їжи накопичено
    double health = 1.0;

    double points = 0;              //кількість зароблених балів
    bool IsSynthesis = false;       //Фаза синтезу
    bool IsProtection = false;      //Фаза Захисту
    bool IsDead = false;            //Чи я помер
    size_t rotting = 0;


    Point position = { 1,1 };       //Моя позиція

    vector<double> memmory;
    void kill();
    infoMove interact(size_t newPosX, size_t newPosY);
private:
    void LoadToWorld();
    void setMarkForMove(size_t answerId);
    double analyzeMove(infoMove move);
    double analyzePos(infoMove move);
    std::vector<double> inputs();
    void Attack(Minion* minion);
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
    static void packetReceive();
    static bool sendPacket();
    static void summonFruit();
    static void SaveColonies(string version);
    static void LoadColonies(string version);
    void LoadColony();
    void SaveColony();
    static void SaveMiniones(string version);
    static void LoadMiniones(string version);

    std::pair<size_t, size_t> getNeuronsCount();

    static std::map<size_t, shared_ptr<Minion>> minionAddresses;
    static double AVRGpoints;
    std::vector<shared_ptr<Minion>> colonyAddresses;
    std::map<shared_ptr<Colony>, float> colonyRelations;
    
    static size_t nextMinionID;

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
    std::pair<size_t, size_t> _neuronsCount;
    string nameColony;
    NeuralNetwork bestMinionBrain;
    //NeuralNetwork colonyBrain;
};

class Spawner
{
public:
    Spawner(shared_ptr<Colony> colony, size_t minPopulation, Point position);
    Spawner(shared_ptr<Colony> colony, size_t minPopulation);

    Point spawnerPosition;
    shared_ptr<Colony> summonSample = nullptr;
private:
    friend class Colony;
    Point generateCord();

};