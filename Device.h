//
// Created by joar on 5/5/16.
//

#ifndef UART2_DEVICE_H
#define UART2_DEVICE_H


#include <string>
#include <list>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>       // std::cin, std::cout
#include "settings.h"
#include "resourceLoader.h"

class Device {
	int nextRangePos;
	int nextRangePosCalib;
	float range[10] = {0};
	float rangeCalib[10] = {0};
	settings *game_settings;
	resourceLoader *game_resources;
	sf::CircleShape circleMeanValue;
	sf::CircleShape dotCircle;
	sf::CircleShape circleShadow;
	sf::CircleShape circleLast;
	sf::Text textAddr;
	sf::Color deviceColor;

	sf::Color deviceColorarray[7] = {
			sf::Color(130 , 130 ,255),
			sf::Color(0   , 255 ,0  ),
			sf::Color(0   , 255 ,255),
			sf::Color(255 , 0   ,0  ),
			sf::Color(255 , 0   ,255),
			sf::Color(255 , 255 ,0  ),
			sf::Color(255 , 255 ,255)
	};
public:
	sf::Text textAnchorPos;
	bool textAnchorPosSelected = false;
	anchor_pos_t anchorPos = anchor_pos_none;
	int deviceNumber;
	sf::Clock clockLastSeen;
	Device(std::string addr, float length, settings *settings, resourceLoader *res);
	void newRange(float length);
	void newCalibRange(float length);

	std::string addr;
	float getMeanRange();
	float getMeanCalibRange();
	float getLastRange();
	float getLastCalibRange();
	float getCalibRangeHighest();
	float getCalibRangeLowest();
	sf::Time getLastSeen();
	void DrawSelf(sf::RenderWindow &window);
	~Device();


	float temp;
};


#endif //UART2_DEVICE_H
