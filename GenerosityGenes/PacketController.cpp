#include "primary_utilities.hpp"
enum typeOfReceivedPacket
{
    ColonyMinionsAreaInit,
    ColonyUpdate,
    MinionsUpdate,
    eat_coefficient
};
Packet packetSend;
bool Colony::sendPacket()
{
    netServer.registerNewClients();
    netServer.sendConnectedClientsRecords();


    packetSend.clear();
    packetSend << typeOfReceivedPacket::ColonyMinionsAreaInit;
    size_t count = allColonies.size();
    packetSend << count;
    for (const auto& colony : allColonies)
    {
        packetSend << colony.second->nameColony;
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
    if(netServer.clientsVec.size()>0)
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
                    string nameOfColony;
                    string value_str;
                    std::pair<int, int> neuronsCount{ 32,28 };
                    Colony* tempColony;
                    receivedDataPacket >> count;
                    //Count of colony
                    for (size_t i = 0; i < count; ++i)
                    {
                        //Load Names of colony
                        receivedDataPacket >> nameOfColony;
                        receivedDataPacket >> neuronsCount.first;
                        receivedDataPacket >> neuronsCount.second;
                        tempColony = new Colony(neuronsCount.first, neuronsCount.second, nameOfColony);

                        //temp->hasSpawner = colonyHasSpawner;
                        //temp->colonyMinSize = colonySize;
                        //if (colonyHasSpawner)new Spawner(temp, colonySize);
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
                            receivedDataPacket >> nameOfColony;
                            receivedDataPacket >> pos.x;
                            receivedDataPacket >> pos.y;
                            colonyArea.insert(pos);
                            worldMap[pos.x][pos.y].minionAddress->myColony = allColonies[nameOfColony];
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