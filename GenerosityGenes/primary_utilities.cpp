#include "primary_utilities.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));
std::map<std::string, Colony*> allColonys;
std::map<Colony*, Spawner*> allActiveSpawners;
std::set<Point,Comp> poolOfFruits;
std::map<Point, sf::Color, Comp> colonyArea;
void worldInitialization()
{
    for (size_t x = 0; x < sizeWorldX; ++x)
    {
        worldMap[x][0].type = border;
        worldMap[x][1].type = border;
        worldMap[x][sizeWorldY - 2].type = border;
        worldMap[x][sizeWorldY - 1].type = border;
    }
    for (size_t y = 1; y < sizeWorldY - 1; ++y)
    {
        worldMap[0][y].type = border;
        worldMap[1][y].type = border;
        worldMap[sizeWorldX - 1][y].type = border;
        worldMap[sizeWorldX - 2][y].type = border;
    }
}


//Це конснтруктор класса Колонія
Colony::Colony(size_t neuronsCountFirst, size_t neuronsCountSecond, std::string name) :
               nameColony(name),
               _neuronsCount({ neuronsCountFirst,neuronsCountSecond }),
               colonyColor(sf::Color(rand() % 256, rand() % 256, rand() % 128 + 128, 255))
{
    coefInitialization();
    allColonys.insert(std::make_pair(nameColony, this));
}
//Це завантажувальний конструктор
Colony::Colony(string name) : nameColony(name)
{
    colonyColor = sf::Color(rand() % 256, rand() % 256, rand() % 128 + 128, 255);
    bestMinionBrain->NeuralNetworkWay = name;
    LoadColony();
    coefInitialization();
    allColonys.insert(std::make_pair(nameColony, this));
}



//Ініціалізація верктору вказивників на своїх мінійонів
std::vector<Minion*> Colony::minionAddresses;

void Colony::coefInitialization()
{
    coef_Synthesis = (double(rand()%200) / 100.0) - 1.0;
    coef_Protection = (double(rand() % 200) / 100.0) - 1.0;
    coef_Born = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_Eat = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackTeam = (double(rand() % 200) / 100.0) - 1.0;
    coef_Border = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerTeam = (double(rand() % 200) / 100.0) - 1.0;
    srand(static_cast<unsigned int>(time(NULL)));
}

//Початок симуляції життя
void Colony::startLife()
{
    size_t count = 0;
    double maxPoints;
    bool leaveOne = false;
    while (true) {
        maxPoints = 0;

        for (size_t fruitI = 0; fruitI < static_cast<size_t>(speedSummonFruit / 1.0f); ++fruitI)
        {
            summonFruit();
        }

        for (size_t i = 0; i < 60; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            render();
        }

        if (isMainWindowOpen == false)
        {
            //Очищення динамічної пам'яті 
            for (auto minion : Colony::minionAddresses)
            {
                delete minion;
            }
            for (const auto colony : allColonys)
            {
                delete colony.second->bestMinionBrain;
            }

            return;
        }


        for (Minion* minion : minionAddresses)
        {
            minion->nextMove();
        }
        for (std::pair<Colony*, Spawner*> spawner : allActiveSpawners)
        {
            if (spawner.first->sizeColony < spawner.second->populationSize)
            {
                spawner.first->createMinion(spawner.second->generateCord());
            }
        }
        ++count;
        if (count == 5)   //(dev tip)
        {
            count == 0;
            for (auto& item : allColonys)
            {
                for (const auto minion : item.second->colonyAddresses)
                {
                        if (minion->points > maxPoints)
                        {
                            maxPoints = minion->points;
                            delete item.second->bestMinionBrain;
                            item.second->bestMinionBrain = &(minion->MyBrain);
                        }
                        minion->points = 0;
                }
                if (item.second->bestMinionBrain != nullptr)
                {
                    for (const auto minion : item.second->colonyAddresses)
                    {
                        minion->MyBrain = *(item.second->bestMinionBrain);
                        if (leaveOne)
                        {
                            minion->MyBrain.mutate();
                        }
                        else leaveOne = true;
                    }
                }
            }
        }
    }
}

void Colony::createMinion()
{
    size_t Xtemp, Ytemp;
    while (true)
    {
        Xtemp = rand() % sizeWorldX;
        Ytemp = rand() % sizeWorldY;
        if (worldMap[Xtemp][Ytemp].type == Types::air)
        {
            Minion* newMinion = new Minion({ Xtemp ,Ytemp }, this);
            minionAddresses.push_back(newMinion);
            colonyAddresses.push_back(newMinion);
            ++MinionSettings::countMiniones;
            return;
        }
    }
}

void Colony::createMinion(Point coordinate)
{
    if (worldMap[coordinate.x][coordinate.y].type == Types::air) {
        Minion* newMinion = new Minion(coordinate, this);
        minionAddresses.push_back(newMinion);
        colonyAddresses.push_back(newMinion);
        ++MinionSettings::countMiniones;
    }
}

void Colony::createMinion(Point coordinate, Minion* parent, double hunger)
{
        if (worldMap[coordinate.x][coordinate.y].type == Types::air)
        {
            Minion* newMinion = new Minion({ coordinate.x ,coordinate.y }, this, &parent->MyBrain, hunger);
            minionAddresses.push_back(newMinion);
            colonyAddresses.push_back(newMinion);
            ++MinionSettings::countMiniones;
            return;
        }
}
void Colony::summonFruit()
{
    size_t Xtemp, Ytemp;
    while (true)
    {
        Xtemp = rand() % sizeWorldX;
        Ytemp = rand() % sizeWorldY;
        if (worldMap[Xtemp][Ytemp].type == Types::air)
        {
            worldMap[Xtemp][Ytemp].type  = Types::fruit; 
            poolOfFruits.insert(Point{ Xtemp,Ytemp });
            return;
        }
    }
}


//Save - Load модуль
void Colony::SaveColony()
{
    bestMinionBrain->SaveAI();
}

void Colony::LoadColony()
{
    bestMinionBrain->LoadAI();
}

void Colony::SaveMiniones(string version)
{
    std::ofstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }

    for (const auto& minion : minionAddresses)
    {
        file << minion->SaveMe() << '\n';
    }

    file.close();
}

void Colony::LoadMiniones(string version)
{
    std::ifstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }

    string dataMinion;
    Minion* temp;
    while (std::getline(file, dataMinion))
    {
        temp = new Minion(dataMinion);
        minionAddresses.push_back(temp);
    }

    file.close();
}
Spawner::Spawner(Colony* colony, size_t minPopulation, Point position) :summonSample(colony), populationSize(minPopulation), spawnerPosition(position)
{
    allActiveSpawners.insert(std::make_pair(summonSample, this));
    worldMap[spawnerPosition.x][spawnerPosition.y].type = Types::spawner;
}
Spawner::Spawner(Colony* colony, size_t minPopulation) :summonSample(colony), populationSize(minPopulation)
{
        size_t Xtemp, Ytemp;
        while (true)
        {
            Xtemp = rand() % sizeWorldX;
            Ytemp = rand() % sizeWorldY;
            if (worldMap[Xtemp][Ytemp].type == Types::air)
            {
                spawnerPosition = { Xtemp ,Ytemp };
                break;
            }
        }
    allActiveSpawners.insert(std::make_pair(summonSample, this));
    worldMap[spawnerPosition.x][spawnerPosition.y].type = Types::spawner;
}
Point Spawner::generateCord()
{
    //(dev tip) потім переписати коли зявиться карта колоній спавнити нового на території колонії
    return { (rand() % 2 == 0) ? spawnerPosition.x - 1 : spawnerPosition.x + 1,(rand() % 2 == 0) ? spawnerPosition.y - 1 : spawnerPosition.y + 1 };
}