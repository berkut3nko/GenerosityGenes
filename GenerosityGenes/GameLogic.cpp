#include "primary_utilities.hpp"
#include "settings.hpp"
std::map<std::string, shared_ptr<Colony>> allColonies;
std::map<shared_ptr<Colony>, shared_ptr<Spawner>> allActiveSpawners;
std::set<Point,Comp> poolOfFruits;
std::set<Point, Comp> poolOfBorders;
std::map<Point, shared_ptr<Colony>, Comp> colonyArea;

vector<std::pair<MinionStackTexture, bool>> minionTexturesPool;

double Colony::AVRGpoints = 0.5;
size_t countMiniones = 0;
Time lastPacketSend; 
Clock elapsedTimePacketSend;

string version = "v0.0.4";

//ініціалізація singleton зміної
bool isStoped = false;

void loadTextures()
{
    std::string folder_path; // Вказуйте шлях до папки тут
    std::filesystem::path currentPath = std::filesystem::current_path() / "Textures";
    folder_path = currentPath.generic_string();
    

    size_t file_count = 0;
    bool state = true;
    // Отримуємо список файлів в папці
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) {
            ++file_count;
        }
    }
    for (size_t i = 0; i < file_count / 4; ++i)
    {
        MinionStackTexture texturePack;
        sf::IntRect size; size.width = 512; size.height = 512;
        state = texturePack.idle        .loadFromFile((folder_path + "/Idle_0" + std::to_string(i) + ".png"), size);
        state = texturePack.dead        .loadFromFile(folder_path + "/Dead_0" + std::to_string(i) + ".png", size);
        state = texturePack.protection  .loadFromFile(folder_path + "/Protection_0" + std::to_string(i) + ".png", size);
        state = texturePack.synthesis   .loadFromFile(folder_path + "/Synthesis_0" + std::to_string(i) + ".png", size);
        minionTexturesPool.push_back({ texturePack, false });
    }
}

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
                if (spawner.first->sizeColony < spawner.second->summonSample->colonyMinSize)
                {
                    Point tempPos = spawner.second->generateCord();
                    if(worldMap[tempPos.x][tempPos.y].type = air)
                        spawner.first->createMinion(tempPos);
                }
            }
            //The method of combining knowladge of minions
            ++count;
            if (count == size_t(retrainingLimit * Colony::AVRGpoints))
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


std::pair<size_t, size_t> Colony::getNeuronsCount()
{
    return _neuronsCount;
}