#include "primary_utilities.hpp"
enum typeOfReceivedPacket
{
    ColonyMinionsAreaInit,
    ColonyUpdate,
    MinionsUpdate,
    eat_coefficient
};
Packet packetSend;
Packet receivedDataPacket;
size_t oldCountUsers=0;

char name[64];
bool colonyHasSpawner = false;

bool Colony::sendPacket()
{
    packetSend.clear();
    if (oldCountUsers < netServer.clientsVec.size()) 
    {
        std::cout << "ColonyMinionsAreaInit" << '\n';
        packetSend << size_t(typeOfReceivedPacket::ColonyMinionsAreaInit);
        packetSend << allColonies.size();
        for (const auto colony : allColonies)
        {
            strcpy_s(name, colony.first.c_str());
            packetSend << name;
            packetSend << colony.second->_neuronsCount.first;
            packetSend << colony.second->_neuronsCount.second;

            colonyHasSpawner = (bool)(allActiveSpawners.find(colony.second) != allActiveSpawners.end());
            packetSend << colonyHasSpawner;
            if (colonyHasSpawner)
            {
                packetSend << allActiveSpawners.find(colony.second)->second->populationSize;
                packetSend << allActiveSpawners.find(colony.second)->second->spawnerPosition.x;
                packetSend << allActiveSpawners.find(colony.second)->second->spawnerPosition.y;
            }


            packetSend << colony.second->coef_AttackEnemy;
            packetSend << colony.second->coef_AttackTeam;
            packetSend << colony.second->coef_Border;
            packetSend << colony.second->coef_Born;
            packetSend << colony.second->coef_Eat;
            packetSend << colony.second->coef_EatClose;
            packetSend << colony.second->coef_EnemyClose;
            packetSend << colony.second->coef_EnemySpawnerClose;
            packetSend << colony.second->coef_Protection;
            packetSend << colony.second->coef_SpawnerEnemy;
            packetSend << colony.second->coef_SpawnerTeam;
            packetSend << colony.second->coef_Synthesis;
            packetSend << colony.second->coef_TeamClose;
            packetSend << colony.second->coef_TeamSpawnerClose;
        }
        strcpy_s(name, "Miniones");

        packetSend << name;
        packetSend << Colony::minionAddresses.size();
        for (std::pair<size_t, shared_ptr<Minion>> minion : minionAddresses)
        {
            packetSend << minion.second->myColony->nameColony;
            packetSend << minion.second->ID;
            packetSend << minion.second->ID;
            packetSend << minion.second->position.x;
            packetSend << minion.second->position.y;
        }
        strcpy_s(name, "Area");
        packetSend << name;
        packetSend << colonyArea.size();
        for (const auto& area : colonyArea)
        {
            strcpy_s(name, area.second->nameColony.c_str());
            packetSend << name;
            packetSend << area.first.x;
            packetSend << area.first.y;
        }
        if (netServer.sendDataToNewbie(packetSend, netServer.clientsVec.size() - oldCountUsers) == Socket::Status::Done)
        packetSend.clear();
    }
        std::cout << "MinionsUpdate" << '\n';
    packetSend << size_t(typeOfReceivedPacket::MinionsUpdate);
    strcpy_s(name, "Miniones");
    packetSend << name;
    packetSend << Colony::minionAddresses.size();
    for (std::pair<size_t, shared_ptr<Minion>> minion : minionAddresses)
    {
        packetSend << minion.second->ID;
        packetSend << minion.second->ID;
        packetSend << minion.second->myColony->nameColony;
        packetSend << minion.second->position.x;
        packetSend << minion.second->position.y;
        receivedDataPacket >> minion.second->IsSynthesis;
        receivedDataPacket >> minion.second->IsProtection;
        receivedDataPacket >> minion.second->IsDead;
        receivedDataPacket >> minion.second->rotting;

    }
    oldCountUsers = netServer.clientsVec.size();
    if (netServer.sendDataToAll(packetSend) == Socket::Status::Done)//(dev tip) sendDataToAll -> newUser / Users
        return true;
    else return false;
}


void Colony::packetReceive()
{
    typeOfReceivedPacket PacketType;
    size_t typeID;
    Colony::SaveColonies(version);
    bool IsSynthesis = false;       //Фаза синтезу
    bool IsProtection = false;      //Фаза Захисту
    bool IsDead = false;            //Чи я помер
    size_t rotting = 0;
    size_t count, ID, ID_sure, populationSize;
    shared_ptr<Colony> tempColony;
    Point pos;
    std::pair<size_t, size_t> neuronsCount{ 32,28 };
    while (isMainWindowOpen == true)
    {
        if (netConnection.receiveData(receivedDataPacket, serverIpAdress, serverPort) == Socket::Done)
        {
            if (receivedDataPacket.getDataSize() > 0) {
                receivedDataPacket >> typeID;
                PacketType = static_cast<typeOfReceivedPacket>(typeID);
                switch (PacketType) {
                case typeOfReceivedPacket::ColonyMinionsAreaInit:
                {
                    allColonies.clear();
                    colonyArea.clear();
                    poolOfFruits.clear();
                    poolOfBorders.clear();
                    Colony::minionAddresses.clear();
                    worldInitialization();
                    //Load Colony

                    receivedDataPacket >> count;
                    //Count of colony
                    for (size_t i = 0; i < count; ++i)
                    {
                        //Load Names of colony
                        receivedDataPacket >> name;
                        receivedDataPacket >> neuronsCount.first;
                        receivedDataPacket >> neuronsCount.second;

                        tempColony = make_shared<Colony>(neuronsCount.first, neuronsCount.second, (string)name);
                        allColonies.insert({ (string)name ,tempColony });

                        receivedDataPacket >> colonyHasSpawner;
                        if (colonyHasSpawner)
                        {
                            receivedDataPacket >> populationSize;
                            receivedDataPacket >> pos.x;
                            receivedDataPacket >> pos.y;
                            allActiveSpawners.insert(std::make_pair(tempColony, make_shared<Spawner>(tempColony, populationSize, pos)));
                        }

                        receivedDataPacket >> tempColony->coef_AttackEnemy;
                        receivedDataPacket >> tempColony->coef_AttackTeam;
                        receivedDataPacket >> tempColony->coef_Border;
                        receivedDataPacket >> tempColony->coef_Born;
                        receivedDataPacket >> tempColony->coef_Eat;
                        receivedDataPacket >> tempColony->coef_EatClose;
                        receivedDataPacket >> tempColony->coef_EnemyClose;
                        receivedDataPacket >> tempColony->coef_EnemySpawnerClose;
                        receivedDataPacket >> tempColony->coef_Protection;
                        receivedDataPacket >> tempColony->coef_SpawnerEnemy;

                        receivedDataPacket >> tempColony->coef_SpawnerTeam;
                        receivedDataPacket >> tempColony->coef_Synthesis;
                        receivedDataPacket >> tempColony->coef_TeamClose;
                        receivedDataPacket >> tempColony->coef_TeamSpawnerClose;
                    }
                    receivedDataPacket >> name;

                    if (strncmp(name, "Miniones", sizeof(name)) == 0) {
                        receivedDataPacket >> count;
                        for (size_t i = 0; i < count; ++i)
                        {
                            receivedDataPacket >> name;
                            if (allColonies.count(string(name)) > 0) {
                                tempColony = allColonies[string(name)];
                            }
                            else {
                                if (allColonies.size() > 0)
                                    tempColony = allColonies.begin()->second;
                                else
                                    return; // call default constructor (dev tip)
                            }
                            receivedDataPacket >> ID;
                            receivedDataPacket >> ID_sure;
                            if (ID == ID_sure) {
                                receivedDataPacket >> pos.x;
                                receivedDataPacket >> pos.y;


                                shared_ptr<Minion> newMinion = make_shared<Minion>(pos, tempColony);
                                newMinion->ID = ID;
                                Colony::minionAddresses.insert({ newMinion->ID,newMinion });
                                tempColony->colonyAddresses.push_back(newMinion);
                            }
                        }
                    }
                    receivedDataPacket >> name;
                    if (strncmp(name, "Area", sizeof(name)) == 0) {
                        receivedDataPacket >> count;
                        for (size_t i = 0; i < count; ++i)
                        {
                            receivedDataPacket >> name;
                            receivedDataPacket >> pos.x;
                            receivedDataPacket >> pos.y;
                            colonyArea.insert({ pos ,allColonies[(string)name] });
                        }
                    }
                    break;
                }
                case typeOfReceivedPacket::MinionsUpdate:
                {
                    receivedDataPacket >> name;
                    Point pos;
                    if (strncmp(name, "Miniones", sizeof(name)) == 0) {
                        receivedDataPacket >> count;
                        for (size_t i = 0; i < count; ++i)
                        {
                            receivedDataPacket >> ID;
                            receivedDataPacket >> ID_sure;
                            if (ID == ID_sure) {
                                receivedDataPacket >> name;
                                receivedDataPacket >> pos.x;
                                receivedDataPacket >> pos.y;
                                //If the data is damaged
                                if (!pos.inRange())
                                {
                                    size_t Xtemp, Ytemp;
                                    while (true)
                                    {
                                        Xtemp = rand() % sizeWorldX;
                                        Ytemp = rand() % sizeWorldY;
                                        if (worldMap[Xtemp][Ytemp].type == Types::air)
                                        {
                                            pos = Point{ Xtemp ,Ytemp };
                                            break;
                                        }
                                    }
                                }

                                if (Colony::minionAddresses.find(ID) == Colony::minionAddresses.end())
                                {
                                    if (allColonies.count(string(name)) > 0) {
                                        tempColony = allColonies[string(name)];
                                    }
                                    else {
                                        if (allColonies.size() > 0)
                                            tempColony = allColonies.begin()->second;
                                        else
                                            return; // call default constructor (dev tip)
                                    }
                                    shared_ptr<Minion> newMinion = make_shared<Minion>(pos, tempColony);
                                    newMinion->ID = ID;
                                    Colony::minionAddresses.insert({ newMinion->ID,newMinion });
                                    tempColony->colonyAddresses.push_back(newMinion);
                                }
                                receivedDataPacket >> IsSynthesis;
                                receivedDataPacket >> IsProtection;
                                receivedDataPacket >> IsDead;
                                receivedDataPacket >> rotting;
                                tempColony = Colony::minionAddresses[ID].get()->myColony;
                                if (Colony::minionAddresses[ID].get()->position != pos)
                                {
                                    Colony::minionAddresses[ID].get()->interact(pos.x, pos.y);
                                }
                                Colony::minionAddresses[ID].get()->IsSynthesis = IsSynthesis;
                                Colony::minionAddresses[ID].get()->IsProtection = IsProtection;
                                Colony::minionAddresses[ID].get()->IsDead = IsDead;
                                Colony::minionAddresses[ID].get()->rotting = rotting;
                            }
                            else std::cout << "\nData loss";
                        }
                    }
                }
                for (auto minion : Colony::minionAddresses)
                {
                    if (minion.second->IsDead) {
                        ++minion.second->rotting;
                        if (minion.second->rotting == 6)
                            minion.second->kill();
                    }
                }
                for (int i = 0; i < 50; ++i)
                {
                    render();
                }
                }
            }
            //else {
            //    isConnected = false;
            //    startLife();
            //    return;
            //}
        }
    }
}
