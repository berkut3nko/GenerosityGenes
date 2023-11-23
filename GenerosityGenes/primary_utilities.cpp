#include "primary_utilities.hpp"
#include "settings.hpp"
std::map<std::string, Colony*> allColonies;
std::map<Colony*, Spawner*> allActiveSpawners;
std::set<Point,Comp> poolOfFruits;
std::set<Point, Comp> colonyArea;
double Colony::AVRGpoints = 0.5;
void worldInitialization()
{
    for (size_t x = 0; x < sizeWorldX; ++x)
    {
        worldMap[x][0].type = border;
        worldMap[x][1].type = border;
        worldMap[x][sizeWorldY - 2].type = border;
        worldMap[x][sizeWorldY - 1].type = border;
    }
    for (size_t y = 2; y < sizeWorldY - 2; ++y)
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
               colonyColor(sf::Color(rand() % 256, rand() % 256, rand() % 128 + 128, 255)),
    bestMinionBrain({ {MinionSettings::minionInputs + sizeMemmory, _neuronsCount.first},
          {_neuronsCount.first, _neuronsCount.second},
          {_neuronsCount.second,MinionSettings::minionOutputs + sizeMemmory} }, nameColony)
{
    coefInitialization();
    allColonies.insert(std::make_pair(nameColony, &(*this)));
}
//Це завантажувальний конструктор
Colony::Colony(string name) : nameColony(name),
bestMinionBrain({ {MinionSettings::minionInputs + sizeMemmory, _neuronsCount.first},
      {_neuronsCount.first, _neuronsCount.second},
      {_neuronsCount.second,MinionSettings::minionOutputs + sizeMemmory} }, nameColony)
{
    colonyColor = sf::Color(rand() % 256, rand() % 256, rand() % 256, 255);
    coefInitialization();
    allColonies.insert(std::make_pair(nameColony, this));
}

Colony::~Colony()
{
    for (auto minion : colonyAddresses)
        delete minion;
}

//Ініціалізація верктору вказивників на своїх мінійонів
std::vector<Minion*> Colony::minionAddresses;

void Colony::coefInitialization()
{
    /*coef_Synthesis = (double(rand()%200) / 100.0) - 1.0;
    coef_Protection = (double(rand() % 200) / 100.0) - 1.0;
    coef_Born = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_Eat = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackTeam = (double(rand() % 200) / 100.0) - 1.0;
    coef_Border = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerTeam = (double(rand() % 200) / 100.0) - 1.0;
    srand(static_cast<unsigned int>(time(NULL)));*/
    coef_Synthesis = 0.1;
    coef_Protection = 0.1;
    coef_Born = 0.6;
    coef_AttackEnemy = 0.7;
    coef_Eat = 1.0;
    coef_AttackTeam = -0.3;
    coef_Border = -1.0;
    coef_SpawnerEnemy = 0.5;
    coef_SpawnerTeam = -1.0;
    coef_TeamClose = 0.3;
    coef_EnemyClose = 0.5;
    coef_TeamSpawnerClose = -0.1;
    coef_EnemySpawnerClose = 0.4;
    coef_EatClose = 0.6;
}

//Початок симуляції життя
void Colony::startLife()
{
    bool newColonyBrain = false;
    size_t count = 0;
    double maxPoints = -1;
    bool leaveOne;
    while (true) {
        maxPoints = -1;
        if (poolOfFruits.size() < ((sizeWorldX * sizeWorldY) / 20)) {
            for (size_t fruitI = 0; fruitI < static_cast<size_t>(speedSummonFruit / 1.0f); ++fruitI)
            {
                summonFruit();
            }
        }
        for (size_t i = 0; i < 60; ++i)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            render();
        }

        if (isMainWindowOpen == false)
        {
            for (auto& item : allColonies)
            {
                leaveOne = false;
                for (const auto minion : item.second->colonyAddresses)
                {
                    if (minion->points > maxPoints)
                    {
                        maxPoints = minion->points;
                        item.second->bestMinionBrain = (minion->MyBrain);
                    }
                    minion->points = -1;
                }
                maxPoints = 0;
            }
            return;
        }

        for (auto colony : allColonies)
        {
            for (Minion* minion : colony.second->colonyAddresses)
            {
                minion->nextMove();
            }
        }
        for (std::pair<Colony*, Spawner*> spawner : allActiveSpawners)
        {
            if (spawner.first->sizeColony < spawner.second->populationSize)
            {
                spawner.first->createMinion(spawner.second->generateCord());
            }
        }
        ++count;
        if (count == size_t(20 * Colony::AVRGpoints))   //(dev tip)
        {
            newColonyBrain = false;

 
            for (auto& item : allColonies)
            {
                leaveOne = false;
                for (const auto minion : item.second->colonyAddresses)
                {
                        if (minion->points > maxPoints)
                        {
                            maxPoints = minion->points;
                            item.second->bestMinionBrain = (minion->MyBrain);
                            newColonyBrain = true;
                        }
                        minion->points = 0;
                }
                if(maxPoints>0.2)
                    Colony::AVRGpoints = (AVRGpoints + (maxPoints/(count/4))) / 2;
                if (newColonyBrain)
                {
                    for (const auto minion : item.second->colonyAddresses)
                    {
                        minion->MyBrain = (item.second->bestMinionBrain);
                        if (leaveOne)
                        {
                            minion->MyBrain.mutate();
                        }
                        else leaveOne = true;
                    }
                }
                maxPoints = 0;
            }
            count = 0;
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
            newMinion->MyBrain.mutate();
            colonyAddresses.push_back(newMinion);
            return;
        }
    }
}

void Colony::createMinion(Point coordinate)
{
    if (worldMap[coordinate.x][coordinate.y].type == Types::air) {
        Minion* newMinion = new Minion(coordinate, this);

        colonyAddresses.push_back(newMinion);
    }
}

void Colony::createMinion(Point coordinate, Minion* parent, double hunger)
{
        if (worldMap[coordinate.x][coordinate.y].type == Types::air)
        {
            Minion* newMinion = new Minion({ coordinate.x ,coordinate.y }, this, &parent->MyBrain, hunger);
            colonyAddresses.push_back(newMinion);
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

void Colony::SaveColonies(string version)
{
    std::ofstream allColoniesFile("SaveColonies_" + version + ".save");
    if (allColoniesFile.is_open()) {
        for (auto colony : allColonies)
        {
            colony.second->bestMinionBrain.NeuralNetworkWay = colony.first;
            colony.second->SaveColony();
            allColoniesFile << colony.first << "\t" << colony.second->_neuronsCount.first << "\t" << colony.second->_neuronsCount.second << "\t"
                << colony.second->coef_AttackEnemy << '\t' << colony.second->coef_AttackTeam << '\t'
                << colony.second->coef_Border << '\t' << colony.second->coef_Born << '\t'
                << colony.second->coef_Eat << '\t' << colony.second->coef_EatClose << '\t'
                << colony.second->coef_EnemyClose << '\t' << colony.second->coef_EnemySpawnerClose << '\t'
                << colony.second->coef_Protection << '\t' << colony.second->coef_SpawnerEnemy << '\t'
                << colony.second->coef_SpawnerTeam << '\t' << colony.second->coef_Synthesis << '\t'
                << colony.second->coef_TeamClose << '\t' << colony.second->coef_TeamSpawnerClose << '\n';
            colony.second->~Colony();
        }
        allColoniesFile.close();
    }
}
void Colony::LoadColonies(string version)
{
    std::ifstream LoadAllColoniesFile("SaveColonies_" + version + ".save");
    if (LoadAllColoniesFile.is_open()) {
        string name,colonyName;
        std::pair<int, int> neuronsCount{32,28};
        

        Colony* temp;
        while (std::getline(LoadAllColoniesFile, name)) {
            std::istringstream iss(name);
            string value_str;
            std::getline(iss, value_str, '\t');
            colonyName = value_str;
            std::getline(iss, value_str, '\t');
            neuronsCount.first = stoi(value_str);
            std::getline(iss, value_str, '\t');
            neuronsCount.second = stoi(value_str);
            temp = new Colony(neuronsCount.first, neuronsCount.second, colonyName);
            temp->LoadColony();
            std::getline(iss, value_str, '\t');
            temp->coef_AttackEnemy = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_AttackTeam = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Border = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Born = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Eat = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EatClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EnemyClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EnemySpawnerClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Protection = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_SpawnerEnemy = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_SpawnerTeam = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Synthesis = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_TeamClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_TeamSpawnerClose = std::stof(value_str);

        }
        LoadAllColoniesFile.close();
    }
}
//Save - Load модуль
void Colony::SaveColony()
{
    bestMinionBrain.SaveAI();
}

void Colony::LoadColony()
{
    bestMinionBrain.LoadAI();
}

void Colony::SaveMiniones(string version)
{
    std::ofstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }
    for(const auto& colony : allColonies)
    for (const auto& minion : colony.second->colonyAddresses)
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
    Colony* colonyAddress;
    Point pos;
    double fat, hunger;
    while (std::getline(file, dataMinion))
    {
        std::istringstream iss(dataMinion);
        string value_str;

        std::getline(iss, value_str, ' ');
        if (allColonies.count(value_str) > 0) {
            colonyAddress = allColonies[value_str];
        }
        else {
            if (allColonies.size() > 0)
                colonyAddress = allColonies.begin()->second;
            else
                return; // call default constructor (dev tip)
        }

        std::getline(iss, value_str, ' ');
        pos.x = stoull(value_str);

        std::getline(iss, value_str, ' ');
        pos.y = stoull(value_str);

        std::getline(iss, value_str, ' ');
        hunger = stod(value_str);

        std::getline(iss, value_str, ' ');
        fat = stod(value_str);

        Minion* temp = new Minion(colonyAddress, pos, fat, hunger);
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