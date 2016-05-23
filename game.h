//
// Created by joar on 1/2/16.
//

#ifndef BF_GAME_H
#define BF_GAME_H

#include <SFML/Graphics.hpp>
#include "states/baseState.h"
#include "resourceLoader.h"
#include "settings.h"
#include <stack>

class game {
public:
	game();
private:

	settings game_settings;
	resourceLoader recources;
	sf::RenderWindow window;
	std::stack<baseState*> gameStateStack;
	sf::Clock clock;

};



#endif //BF_GAME_H
