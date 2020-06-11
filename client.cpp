// VSYProjekt2.0.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "pch.h"
#include <iostream>
#include <list>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

const unsigned short PORT = 5000;
const unsigned short BACKUPPORT = 5001;
const std::string IPADDRESS("localhost");//change to suit your needs
const std::string BACKUPIP("localhost");
int Heartbeatcount = 0;
bool connected = false;
bool timeout = false;




int main() {

;
	sf::IpAddress ip = sf::IpAddress(IPADDRESS);
	sf::IpAddress ipbackup = sf::IpAddress(IPADDRESS);
	sf::TcpSocket socket;
	bool done = false;
	std::string id;
	std::string text = "";
	std::cout << "Enter online id (Without Spaces!):";
	std::cin >> id;
	socket.connect(ip, PORT);
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), ("Chat of the User: "+id));
	std::vector<sf::Text> chat;
	sf::Packet packet;
	packet << id;
	socket.send(packet);
	socket.setBlocking(false);
	sf::Font font;
	font.loadFromFile("test.ttf");
	int msgcount = 0;

	connected = true;
	
	while (window.isOpen()) {
		//std::cout <<"socket ip:"<<socket.getRemoteAddress();
		sf::Event Event;
		while (window.pollEvent(Event)) {
			switch (Event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (Event.key.code == sf::Keyboard::Escape)
					window.close();
				else if (Event.key.code == sf::Keyboard::Return)
				{
					sf::Packet packet;
					packet << id + ": " + text;
					socket.send(packet);
					sf::Text displayText(text, font, 20);
					displayText.setFillColor(sf::Color::Green);
					chat.push_back(displayText);
					text = "";
					msgcount++;
					if (msgcount >= 20) {
						chat.erase(chat.begin());

					}
				}

				break;

			case sf::Event::TextEntered:

				text += Event.text.unicode;
				if ((Event.text.unicode == '\b') && (text.size() >= 0)) {
					text.erase(text.size() - 1, 1);
					if (text.size() > 0) {
						text.erase(text.size() - 1, 1);
					}
				}

				break;


			}
		}
		sf::Packet packet;
		socket.receive(packet);

		std::string temptext;
		if (packet >> temptext)
		{
			if (temptext != "Server iz da") {



				msgcount++;
				sf::Text displayText(temptext, font, 20);
				displayText.setFillColor(sf::Color::Red);
				if (msgcount >= 20) {
					chat.erase(chat.begin());

				}
				chat.push_back(displayText);
			}
			else
				std::cout << "Server iz immernoch da" << std::endl;
			
		socket.setBlocking(false);
		
		}
		
		if (socket.receive(packet) == sf::Socket::Disconnected) {
			std::cout << "(Server): Server appeareantly crashed, switching to another one" << std::endl;
			sf::sleep(sf::seconds(2));
			socket.disconnect();
			socket.connect(BACKUPIP, BACKUPPORT);
			socket.send(packet);
			socket.setBlocking(false);
		}
		int i = 0;
		for (i; i < chat.size(); i++)
		{
			chat[i].setPosition(0, i * 20);
			window.draw(chat[i]);
		}
		sf::Text drawText(text, font, 20);
		drawText.setOutlineColor(sf::Color::Red);
		drawText.setPosition(0, i * 20);
		window.draw(drawText);
		window.display();
		window.clear();



	}

	return 0;
}
