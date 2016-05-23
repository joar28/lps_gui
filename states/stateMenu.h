//
// Created by joar on 1/2/16.
//

#ifndef BF_STATEMENU_H
#define BF_STATEMENU_H


#include "baseState.h"

class stateMenu : public baseState {

public:
	stateMenu(settings *settings, resourceLoader *res);
	virtual void EventKeyboard(sf::Event event);
	virtual void Process(sf::Time TimeSinceLastTick);
	virtual void Draw(sf::RenderWindow &window);
	virtual baseState *NextState();

private:
	sf::Text heading;
#define antall_texten 5
	sf::Text texten[antall_texten];
	baseState *stateNext;

public:
	virtual void EventMouse(sf::Event event);
};


#endif //BF_STATEMENU_H
