//
// Created by joar on 5/9/16.
//

#ifndef UART_GUI_STATE2D_H
#define UART_GUI_STATE2D_H


#include "baseState.h"
#include "../settings.h"
#include "../resourceLoader.h"
#include "../Device.h"
#include <set>
#include <thread>

class state2D  : public baseState {

	void HasTimedOut();
	baseState *stateNext;
	//settings *game_settings;
	sf::Text textMode;
	sf::Text textIpEth;
	sf::Text textIpWlan;
	sf::Text textCoordinat;
	sf::CircleShape tagPos;
	float anchorDistance;
	std::set<Device*> devices;

	std::thread *udpServerThread;
public:
	double coX, coY, coZ;
	state2D(settings *settings, resourceLoader *res);
	virtual void EventKeyboard(sf::Event event);
	virtual void EventMouse(sf::Event event);
	virtual void Process(sf::Time TimeSinceLastTick);
	virtual void Draw(sf::RenderWindow &window);
	virtual baseState *NextState();
	~state2D();
};


#endif //UART_GUI_STATE2D_H
