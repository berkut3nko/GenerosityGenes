
#include "Client.hpp"
NetworkClient::NetworkClient()
{
}

Socket::Status NetworkClient::init(unsigned short preferablePort)
{
	Socket::Status status = dataSocket.bind(preferablePort);

	if (status == Socket::Status::Done)
	{
		return Socket::Status::Done;
	}
	else
	{
		do
		{
			unsigned short newPort = Socket::AnyPort;

			status = dataSocket.bind(newPort);

		} while (status != Socket::Done);

		return Socket::Status::Done;
	}
}

Socket::Status NetworkClient::registerOnServer(IpAddress serverIp, unsigned short serverRegPort, std::string clientName)
{
	if (connectRegTcpSocket(serverIp, serverRegPort) != Socket::Status::Done)
		return Socket::Status::Error;

	if (sendClientRecipientData(clientName) != Socket::Status::Done)
		return Socket::Status::Error;

	if (recieveDedicatedDataServerPort() != Socket::Status::Done)
		return Socket::Status::Error;

}

Socket::Status NetworkClient::receiveConnectedClientsNames(std::vector<std::string>& namesVec)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;

	if (regSocket.receive(tempPacket) == Socket::Status::Done)
	{
		if (tempPacket.getDataSize() > 0)
		{
			while (!tempPacket.endOfPacket())
			{
				std::string name;
				if (tempPacket >> name)
				{
					if (name == "FIRST")
					{
						"receiveConnectedClientsNames(): No clients are connected, you are first\n";
						return Socket::Status::Done;
					}
					namesVec.push_back(name);
				}
				else
				{
					return Socket::Status::Error;
				}
			}
			/*for (int i = 0; i < namesVec.size(); i++)
			{
				cout << namesVec[i];
			}
			cout << endl;*/
			return Socket::Status::Done;

		}
	}


	return Socket::Status::Error;
}

Socket::Status NetworkClient::receiveData(Packet& dataPacket, IpAddress S_Ip, unsigned short S_dataPort)
{
	if (dataSocket.isBlocking())dataSocket.setBlocking(false);

	if (dataSocket.receive(dataPacket, S_Ip, S_dataPort) == Socket::Status::Done)
	{
		if (dataPacket.getDataSize() > 0)
		{
			//cout << "receiveData(): Data received\n";
			return Socket::Status::Done;
		}
		else
		{
			return Socket::Status::Error;
		}
	}
	return Socket::Status::NotReady;
}

Socket::Status NetworkClient::sendData(Packet dataPacket)
{
	if (sendRateTimer.getElapsedTime().asMilliseconds() > sendRate)
	{
		if (dataSocket.isBlocking())dataSocket.setBlocking(false);

		if (sendPacket.getDataSize() == 0) sendPacket = dataPacket;

		IpAddress tempIp = S_Ip;
		unsigned short tempDataPort = S_dataPort;
		if (dataSocket.send(sendPacket, tempIp, tempDataPort) == Socket::Status::Done)
		{
			sendPacket.clear();
			sendRateTimer.restart();
			return Socket::Status::Done;
		}
		else return Socket::Status::NotReady;
	}
	else return Socket::Status::NotReady;
}

void NetworkClient::setSendFreq(Int32 milliseconds)
{
	sendRate = milliseconds;
}

Socket::Status NetworkClient::connectRegTcpSocket(IpAddress serverIp, unsigned short serverRegPort)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	if (regSocket.connect(serverIp, serverRegPort, seconds(10)) == Socket::Status::Done)
	{
		S_Ip = serverIp;
		S_dataPort = serverRegPort;
		return Socket::Status::Done;
	}
	else
	{
		return Socket::Status::Error;
	}
}

Socket::Status NetworkClient::sendClientRecipientData(std::string clientName)
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;
	tempPacket << clientName << static_cast<Uint16>(dataSocket.getLocalPort());

	if (regSocket.send(tempPacket) == Socket::Status::Done)
	{
		return Socket::Status::Done;
	}
	else
	{
		return Socket::Status::Error;
	}
}

Socket::Status NetworkClient::recieveDedicatedDataServerPort()
{
	if (!regSocket.isBlocking()) regSocket.setBlocking(true);

	Packet tempPacket;

	if (regSocket.receive(tempPacket) == Socket::Status::Done)
	{
		if (tempPacket.getDataSize() > 0)
		{
			if (tempPacket.getDataSize() == sizeof(Uint16))
			{
				if (tempPacket >> S_dataPort)
				{
					return Socket::Status::Done;
				}
			}
		}
	}

	return Socket::Status::Error;
}
