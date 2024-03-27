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
char name[64];
bool Colony::sendPacket()
{



    packetSend.clear();
    packetSend << size_t(typeOfReceivedPacket::ColonyMinionsAreaInit);
    packetSend << allColonies.size();
    for (const auto& colony : allColonies)
    {
        packetSend << strcpy_s(name,colony.second->nameColony.c_str());
        packetSend << colony.second->_neuronsCount.first;
        packetSend << colony.second->_neuronsCount.second;

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
    packetSend << "Miniones";
    packetSend << MinionSettings::countMiniones;
    for (const auto& colony : allColonies)
    {
        for (Minion* minion : colony.second->colonyAddresses)
        {
            packetSend << minion->myColony->nameColony;
            packetSend << minion->position.x;
            packetSend << minion->position.y;
        }
    }
    packetSend << "Area";
    packetSend << colonyArea.size();
    for (const auto& area : colonyArea)
    {
        packetSend << worldMap[area.x][area.y].minionAddress->myColony->nameColony;
        packetSend << area.x;
        packetSend << area.y;
    }
    if (netServer.sendDataToAll(packetSend) == Socket::Status::Done)
    {
        return true;
    }
    else return false;

}
void Colony::startListen()
{
    poolOfFruits.clear();
    poolOfBorders.clear();
    colonyArea.clear();
    for (const auto& colony : allColonies)
    {
        for (Minion* minion : colony.second->colonyAddresses)
        {
            minion->kill();
        }
    }
    while (isMainWindowOpen == true) {
        Packet receivedDataPacket;
        typeOfReceivedPacket PacketType;
        size_t typeID;
        if (netConnection.receiveData(receivedDataPacket, serverIpAdress, serverPort) == Socket::Done) 
        {
            if (receivedDataPacket.getDataSize() > 0) {
                receivedDataPacket >> typeID;
                PacketType = static_cast<typeOfReceivedPacket>(typeID);
                switch (PacketType) {
                case typeOfReceivedPacket::ColonyMinionsAreaInit:
                {
                    //Load Colony
                    bool colonyHasSpawner = false;
                    size_t count;
                    string value_str;
                    std::pair<int, int> neuronsCount{ 32,28 };
                    Colony* tempColony;
                    receivedDataPacket >> count;
                    //Count of colony
                    for (size_t i = 0; i < count; ++i)
                    {
                        //Load Names of colony
                        receivedDataPacket >> name;
                        receivedDataPacket >> neuronsCount.first;
                        receivedDataPacket >> neuronsCount.second;
                        tempColony = new Colony(neuronsCount.first, neuronsCount.second, (string)name);

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
                    //Load Minions pos
                    Colony* colonyAddress;
                    Point pos;
                    receivedDataPacket >> value_str;
                    if (value_str == "Miniones") {
                        receivedDataPacket >> count;
                        for (size_t i = 0; i < count; ++i)
                        {
                            receivedDataPacket >> value_str;
                            if (allColonies.count(value_str) > 0) {
                                colonyAddress = allColonies[value_str];
                            }
                            else {
                                if (allColonies.size() > 0)
                                    colonyAddress = allColonies.begin()->second;
                                else
                                    return; // call default constructor (dev tip)
                            }

                            receivedDataPacket >> pos.x;
                            receivedDataPacket >> pos.y;

                            Minion* temp = new Minion(pos, colonyAddress);
                            minionAddresses.push_back(temp);
                        }
                    }
                    receivedDataPacket >> value_str;
                    if (value_str == "Area") {
                        receivedDataPacket >> count;
                        for (size_t i = 0; i < count; ++i)
                        {
                            receivedDataPacket >> name;
                            receivedDataPacket >> pos.x;
                            receivedDataPacket >> pos.y;
                            colonyArea.insert(pos);
                            worldMap[pos.x][pos.y].minionAddress->myColony = allColonies[(string)name];
                        }
                    }
                    break;
                }
                case typeOfReceivedPacket::MinionsUpdate: {

                    break;
                }
                default:
                    break;
                }

            }
            render();

        }
            else
        {
            isConnected = false;
            startLife();
            return;
        }
    }
    //(dev tip) un init connection or destruct
}