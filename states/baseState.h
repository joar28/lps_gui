//
// Created by joar on 1/2/16.
//

#ifndef BF_BASESTATE_H
#define BF_BASESTATE_H
#include <SFML/Graphics.hpp>
#include "../settings.h"
#include "../resourceLoader.h"

class baseState {

public:
	virtual void EventKeyboard(sf::Event event) = 0;
	virtual void EventMouse(sf::Event event) = 0;
	virtual void Process(sf::Time TimeSinceLastTick) = 0;
	virtual void Draw(sf::RenderWindow &window) = 0;
	virtual baseState* NextState() = 0;
	virtual ~baseState(){}
	settings* game_settings;
	resourceLoader *game_resources;
//	SoundEffects* sound;
//	StateSettings* statesettings;
};


#endif //BF_BASESTATE_H
