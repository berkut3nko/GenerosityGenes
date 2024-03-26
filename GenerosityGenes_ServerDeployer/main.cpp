#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "Server.hpp"

using namespace sf;
using namespace std;

NetworkServer netServer;


int main()
{
	netServer.init();


	Packet packet;
	packet << "DATA";
	for (int i = 0; i < netServer.clientsVec.size(); i++)
	{
		packet << netServer.clientsVec[i].name << netServer.clientsVec[i].pos.x << netServer.clientsVec[i].pos.y;
	}

	while (true)
	{
		netServer.registerNewClients();
		netServer.sendConnectedClientsRecords();

		if (netServer.sendDataToAll(packet) == Socket::Status::Done)
		{
			packet.clear();
			packet << "DATA";
			for (int i = 0; i < netServer.clientsVec.size(); i++)
			{
				packet << netServer.clientsVec[i].name << netServer.clientsVec[i].pos.x << netServer.clientsVec[i].pos.y;
			}
		}


		unsigned int receivedClientIndex;
		if (netServer.receiveData(receivedClientIndex) == Socket::Status::Done)
		{
			if (netServer.clientsVec[receivedClientIndex].rDataPacket.getDataSize() > 0)
			{
				string s;
				if (netServer.clientsVec[receivedClientIndex].rDataPacket >> s)
				{
					if (s == "DATA")
					{
						float x, y;
						if (netServer.clientsVec[receivedClientIndex].rDataPacket >> x)
						{
							netServer.clientsVec[receivedClientIndex].pos.x = x;
						}
						if (netServer.clientsVec[receivedClientIndex].rDataPacket >> y)
						{
							netServer.clientsVec[receivedClientIndex].pos.y = y;
						}
						netServer.clientsVec[receivedClientIndex].rDataPacket.clear();
						netServer.clientsVec[receivedClientIndex].rDataPacket.clear();
					}
				}
			}
		}
	}


	getchar();
	return 0;
}
