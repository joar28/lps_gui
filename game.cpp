//
// Created by joar on 1/2/16.
//

#include <iostream>
#include "game.h"
#include "states/stateMenu.h"



game::game() : game_settings(), recources(){

	//Lager vinduet som brukes gjennom hele spillet
	window.create(sf::VideoMode(game_settings.windowX, game_settings.windowY), "LSP GUI ting");
	window.setVerticalSyncEnabled(true);

	game_settings.window = &window;
	gameStateStack.push(new stateMenu(&game_settings, &recources));
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					//Det som skjer visst vindu blir krysset ut.
					window.close();
					break;
				case sf::Event::MouseButtonPressed:
				case sf::Event::MouseButtonReleased:
				case sf::Event::MouseEntered:
				case sf::Event::MouseLeft:
				case sf::Event::MouseMoved:
				case sf::Event::MouseWheelMoved:
					gameStateStack.top()->EventMouse(event);
					break;
				default:
					gameStateStack.top()->EventKeyboard(event);
					break;
			}
		}
		baseState *nxtStatePtr = gameStateStack.top()->NextState();
		if (nxtStatePtr == nullptr){
			//popper øverste state på stacken for å gå tilbake til forrige state
			delete gameStateStack.top();
			gameStateStack.pop();
			std::cout << "GameState Popped!!" << std::endl;
			if (gameStateStack.size() == 0)
				return;
		} else if (gameStateStack.top() != nxtStatePtr){
			//New state on the stack
			std::cout << "GameState Pushed!" << std::endl;
			gameStateStack.push(nxtStatePtr);
		}


		//Få tiden siden sist frame ble tegnet.
		sf::Time time = clock.getElapsedTime();

		//Reset klokke for å bli klar til neste loop
		clock.restart();

		//Kjør process på aktiv state
		gameStateStack.top()->Process(time);



		window.clear(sf::Color(1,32,67));
		gameStateStack.top()->Draw(window);
		window.display();
	}
}
