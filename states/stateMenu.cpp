//
// Created by joar on 1/2/16.
//

#include <iostream>
#include "stateMenu.h"
#include "../game.h"
#include "../resourceLoader.h"
#include "state2D.h"
#include <SFML/Graphics.hpp>

sf::RenderWindow *windoww = nullptr;

stateMenu::stateMenu(settings *settings, resourceLoader *res){
	stateNext = this;
	game_settings = settings;
	game_resources = res;
	heading.setFont(game_resources->playfair_regular);
	heading.setString("LPS/IPS");
	heading.setCharacterSize(75);
	heading.setColor(sf::Color::White);
	heading.setPosition(200,30);

	for (int i = 0; i < antall_texten; i++){
		texten[i].setFont(game_resources->playfair_regular);
		texten[i].setCharacterSize(35);
		texten[i].setColor(sf::Color::White);
		texten[i].setPosition(50,130+(50*i));
	}
	int j = 0;
	texten[j++].setString("Show 2D");
	texten[j++].setString("asasdasdasdd");
	texten[j++].setString("Options");
	texten[j++].setString("asd");

}

void stateMenu::EventMouse(sf::Event event) {
	switch (event.type){
		case sf::Event::MouseButtonPressed:
			for (int i = 0; i < antall_texten; i++){
				float textLeft = texten[i].getGlobalBounds().left;
				float textRight = textLeft + texten[i].getGlobalBounds().width;
				float textTop = texten[i].getGlobalBounds().top;
				float textBottom = textTop + texten[i].getGlobalBounds().height;

				if ((textLeft <= sf::Mouse::getPosition(*windoww).x && textRight  >= sf::Mouse::getPosition(*windoww).x) &&
				   (textTop   <= sf::Mouse::getPosition(*windoww).y && textBottom >= sf::Mouse::getPosition(*windoww).y)){
					if(i == 0){
						std::cout << "Show 2D" << std::endl;
						this->stateNext = new state2D(game_settings, game_resources);
					}
				}else {
				}
			}
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseEntered:
			break;
		case sf::Event::MouseLeft:
			break;
		case sf::Event::MouseMoved:
			for (int i = 0; i < antall_texten; i++){
				float textLeft = texten[i].getGlobalBounds().left;
				float textRight = textLeft + texten[i].getGlobalBounds().width;
				float textTop = texten[i].getGlobalBounds().top;
				float textBottom = textTop + texten[i].getGlobalBounds().height;

				if ((textLeft <= event.mouseMove.x && textRight  >= event.mouseMove.x) &&
				   (textTop   <= event.mouseMove.y && textBottom >= event.mouseMove.y)){
					texten[i].setCharacterSize(50);
					texten[i].setColor(sf::Color::Yellow);
					texten[i].setPosition(50,120+(50*i));
				}else {
					texten[i].setCharacterSize(35);
					texten[i].setColor(sf::Color::White);
					texten[i].setPosition(50,130+(50*i));
				}
			}
			break;
		case sf::Event::MouseWheelMoved:
			break;
		default:
			break;
	}
}

void stateMenu::EventKeyboard(sf::Event event) {
//	std::cout << "Menu: event" << std::endl;
//	switch (event.){
//	}
}

void stateMenu::Process(sf::Time TimeSinceLastTick) {
}

void stateMenu::Draw(sf::RenderWindow &window) {
	window.draw(heading);
	if(windoww == nullptr){
		windoww = &window;
	}
	for (int i = 0; i < antall_texten; i++){
		window.draw(texten[i]);
	}

}

baseState *stateMenu::NextState() {
	return stateNext;
}
