#include "primary_utilities.hpp"
#include "settings.hpp"
std::map<std::string, shared_ptr<Colony>> allColonies;
std::map<shared_ptr<Colony>, shared_ptr<Spawner>> allActiveSpawners;
std::set<Point,Comp> poolOfFruits;
std::set<Point, Comp> poolOfBorders;
std::map<Point, shared_ptr<Colony>, Comp> colonyArea;
double Colony::AVRGpoints = 0.5;
size_t countMiniones = 0;
Time lastPacketSend; 
Clock elapsedTimePacketSend;

string version = "v0.0.3";

//ініціалізація singleton зміної
bool isStoped = false;

void worldInitialization()
{
    for (auto& line : worldMap)
    {
        for (auto& object : line)
        {
            object.type = air;
        }
    }
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
    for (auto colony : allColonies)
    {
        colonyRelations.insert(std::make_pair(colony.second,0.5f));
        colony.second->colonyRelations.insert(std::make_pair(this, 0.5f));
    }
    coefInitialization();
}

Colony::~Colony()
{
    for (auto minion : colonyAddresses)
        minion->kill();
}

//Ініціалізація верктору вказивників на своїх мінійонів
std::map<size_t, shared_ptr<Minion>> Colony::minionAddresses;
//Унікальні індефікатори
size_t Colony::nextMinionID = 0;

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
    coef_Synthesis = 0.1f;
    coef_Protection = 0.1f;
    coef_Born = 0.6f;
    coef_AttackEnemy = 0.7f;
    coef_Eat = 1.0f;
    coef_AttackTeam = -0.3f;
    coef_Border = -1.0f;
    coef_SpawnerEnemy = 0.5f;
    coef_SpawnerTeam = -1.0f;
    coef_TeamClose = 0.3f;
    coef_EnemyClose = 0.5f;
    coef_TeamSpawnerClose = -0.1f;
    coef_EnemySpawnerClose = 0.4f;
    coef_EatClose = 0.6f;
}

//Початок симуляції життя
void Colony::startLife()
{
    bool newColonyBrain = false;
    size_t count = 0;
    double maxPoints = -1;
    bool leaveOne;
    while (!isConnected) {
        maxPoints = -1;
        if(!isStoped)
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
        if (!isStoped) 
        {
            for (auto colony : allColonies)
            {
                for (shared_ptr<Minion> minion : colony.second->colonyAddresses)
                {
                    minion->nextMove();
                }
            }
            for (std::pair<shared_ptr<Colony>, shared_ptr<Spawner>> spawner : allActiveSpawners)
            {
                if (spawner.first->sizeColony < spawner.second->populationSize)
                {
                    spawner.first->createMinion(spawner.second->generateCord());
                }
            }

            //if (Colony::minionAddresses.size() > MinionSettings::countMiniones)
            //{
            //    bool toErase=
            //    for (auto& minion : Colony::minionAddresses)
            //    {
            //        
            //    }
            //}
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
                    if (maxPoints > 0.2)
                        Colony::AVRGpoints = (AVRGpoints + (maxPoints / (count / 4))) / 2;
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
        if (isServerOpened) {
            netServer.registerNewClients();
            netServer.sendConnectedClientsRecords();
            if (netServer.clientsVec.size() > 0)
            if (sendPacket()) {
                lastPacketSend = elapsedTimePacketSend.getElapsedTime();
                std::cout << "Packet was sended\n";
                elapsedTimePacketSend.restart();
            }
        }
    } 
    if (isConnected)
    {
        std::cout << "Listen was Started\n";
        packetReceive();
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
            shared_ptr<Minion> newMinion = make_shared<Minion>(Point{ Xtemp ,Ytemp }, allColonies[this->nameColony]);
            Colony::minionAddresses.insert({ newMinion->ID,newMinion });
            newMinion->MyBrain.mutate();
            colonyAddresses.push_back(newMinion);
            return;
        }
    }
}

void Colony::createMinion(Point coordinate)
{
    if (worldMap[coordinate.x][coordinate.y].type == Types::air) {
        shared_ptr<Minion> newMinion = make_shared<Minion>(coordinate, allColonies[this->nameColony]);
        Colony::minionAddresses.insert({ newMinion->ID,newMinion });
        colonyAddresses.push_back(newMinion);
    }
}

void Colony::createMinion(Point coordinate, Minion* parent, double hunger)
{
        if (worldMap[coordinate.x][coordinate.y].type == Types::air)
        {
            shared_ptr<Minion> newMinion = make_shared<Minion>(Point{ coordinate.x ,coordinate.y }, allColonies[this->nameColony], &parent->MyBrain, hunger);
            Colony::minionAddresses.insert({ newMinion->ID,newMinion });
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
            allColoniesFile << colony.first << "\t";
            allColoniesFile
                << colony.second->_neuronsCount.first << '\t' << colony.second->_neuronsCount.second << '\t'
                << colony.second->coef_AttackEnemy << '\t' << colony.second->coef_AttackTeam << '\t'
                << colony.second->coef_Border << '\t' << colony.second->coef_Born << '\t'
                << colony.second->coef_Eat << '\t' << colony.second->coef_EatClose << '\t'
                << colony.second->coef_EnemyClose << '\t' << colony.second->coef_EnemySpawnerClose << '\t'
                << colony.second->coef_Protection << '\t' << colony.second->coef_SpawnerEnemy << '\t'
                << colony.second->coef_SpawnerTeam << '\t' << colony.second->coef_Synthesis << '\t'
                << colony.second->coef_TeamClose << '\t' << colony.second->coef_TeamSpawnerClose << '\n';
        }
        allColoniesFile.close();
    }
}
void Colony::LoadColonies(string version)
{
    std::ifstream LoadAllColoniesFile("SaveColonies_" + version + ".save");
    if (LoadAllColoniesFile.is_open()) {
        string name,colonyName;
        bool colonyHasSpawner = false;
        std::pair<int, int> neuronsCount{32,28};
        

        shared_ptr<Colony> temp;
        while (std::getline(LoadAllColoniesFile, name)) {
            std::istringstream iss(name);
            string value_str;
            std::getline(iss, value_str, '\t');
            colonyName = value_str;
            //std::getline(iss, value_str, '\t');
            //colonyHasSpawner = value_str == "true" ? true : false;
            //std::getline(iss, value_str, '\t');
            //colonySize = std::stoi(value_str);

            srand(rand() % 241);
            std::getline(iss, value_str, '\t');
            neuronsCount.first = stoi(value_str);
            std::getline(iss, value_str, '\t');
            neuronsCount.second = stoi(value_str);
            temp = make_shared<Colony>(neuronsCount.first, neuronsCount.second,colonyName);
            temp->LoadColony();
            //temp->hasSpawner = colonyHasSpawner;
            //temp->colonyMinSize = colonySize;
            //if (colonyHasSpawner)new Spawner(temp, colonySize);
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
    shared_ptr<Colony> colonyAddress;
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

        shared_ptr<Minion> newMinion = make_shared<Minion>(colonyAddress, pos, fat, hunger);
        minionAddresses.insert({ newMinion->ID,newMinion });
    }

    file.close();
}
Spawner::Spawner(shared_ptr<Colony> colony, size_t minPopulation, Point position) :summonSample(colony), populationSize(minPopulation), spawnerPosition(position)
{
    if (colony->hasSpawner == false) {
        worldMap[spawnerPosition.x][spawnerPosition.y].type = Types::spawner;
        colony->hasSpawner = true;
        colony->colonyMinSize = minPopulation;
    }
    return;

}
Spawner::Spawner(shared_ptr<Colony> colony, size_t minPopulation) :summonSample(colony), populationSize(minPopulation)
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
std::pair<size_t, size_t> Colony::getNeuronsCount()
{
    return _neuronsCount;
}