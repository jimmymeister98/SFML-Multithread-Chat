
// VSYProjekt2.0.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "pch.h"
#include <iostream>
#include <list>
#include <thread>
#include <SFML/Network.hpp>
#include <mutex>

unsigned short PORT = 5001;
const std::string IPADDRESS("localhost");//change to suit your needs

std::string msgSend;
sf::Mutex test;

sf::TcpSocket socket;
sf::Mutex globalMutex;
bool quit = false;
std::string Name;


//***************************************************************************************

void heartbeat(std::vector<sf::TcpSocket*> &clientlist)
{
	while(true){ 
		for (int i = 0; i < clientlist.size(); i++) {
			std::string Heartbeat = "Server iz da";
			sf::Packet Heartbeatpacket;
			Heartbeatpacket << Heartbeat;
			clientlist[i]->send(Heartbeatpacket);
			std::cout << "Heartbeat Sent!"<<std::endl;
		}
	}

}
sf::SocketSelector selector;
sf::TcpListener listener;
std::vector<sf::TcpSocket*> clients;

void writestuff(){				
				sf::TcpSocket *socket = new sf::TcpSocket;
				listener.accept(*socket);
				sf::Packet packet;
				std::string id;
				if (socket->receive(packet) == sf::Socket::Done)
					packet >> id;
				std::cout << id << "has joined the chat!" << std::endl;
				clients.push_back(socket);
				socket->setBlocking(false);
				selector.add(*socket);
}

void DoStuff(std::string Name) {
	sf::Thread* writethread = 0;
	
	
	
	listener.listen(PORT);
	selector.add(listener);
	static std::string oldMsg;
	
	//sf::Thread* hbthread = 0;
	//hbthread = new sf::Thread(&heartbeat, clients);
	//hbthread->launch();
	writethread = new sf::Thread(&writestuff);
	while (!quit)
	{
		if (selector.wait());
		{

			// Test the listener
			if (selector.isReady(listener))
			{
				// The listener is ready: there is a pending connection
				std::lock_guard<sf::Mutex> lock(globalMutex);//Sperrt den mutex, entsperrt wenn aus if raus(Mutex entsperrt wenn out of scope)
				//globalMutex.lock();
					//Lock the Thread while configuring and adding the connected socket
				writethread->launch();
				
				//globalMutex.unlock();
			}
			else
			{
				for (int i = 0; i < clients.size(); i++)
				{

					if (selector.isReady(*clients[i]))
					{

						sf::Packet packet, sendPacket;
						if (clients[i]->receive(packet) == sf::Socket::Done)
						{	
							globalMutex.lock(); //Lock mutex so the text cannot be altered
							std::string text;
							packet >> text;
							sendPacket << text;
							for (int j = 0; j < clients.size(); j++)
							{
								if (i != j)
								{
									clients[j]->send(sendPacket);
								}
							}
							globalMutex.unlock();
						}
						if (clients[i]->receive(packet) == sf::Socket::Disconnected)
						{
							std::cout << "Client disconnected" << std::endl;
							std::cout << "list size: " << clients.size();
							globalMutex.lock();//Secure list access
							selector.remove(*clients[i]);
							clients[i]->disconnect();
							//delete(&clients[i]);
							clients.erase(clients.begin() + i);
							globalMutex.unlock();
							std::cout << " then: " << clients.size() << std::endl;
						}
					}
				}
			}

		}
		//schickt heartbeat nach 2 sek oder nach packet;
	
	}
	//if (hbthread) {
	//		hbthread->wait();
	//		delete hbthread;
	//	}

}

int main(int argc, char* argv[])
{
	sf::Thread* thread = 0;
	std::cout << "Currently available Ports:5000 (Mainserver) 5001:(Backup)";
	std::cout << "Select Port: ";
	std::cin >> PORT;
	if ((PORT == 5000) || (PORT == 5001)) {

		std::cout << "\n Server started! The IP is :" << IPADDRESS << ":" << PORT << std::endl;


		thread = new sf::Thread(&DoStuff, Name); //std bind zum überbrücken dass scheiss sfml nur ein argument beim Threading unterstützt
		thread->launch();

		//while (!quit)
		//{
		//	GetInput();
		//}

		if (thread)
		{
			thread->wait();
			delete thread;

		}
	}
	else
	{
		std::cout << "INVALID PORT!" << std::endl;
		return 2;
	}
	return 0;
}
