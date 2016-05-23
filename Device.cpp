//
// Created by joar on 5/5/16.
//

#include <iomanip>
#include "Device.h"
#include "settings.h"


Device::Device(std::string addr, float length, settings *settings, resourceLoader *res) {
	this->game_settings = settings;
	this->game_resources = res;
	std::cout << "Adding 0x" << addr << std::endl;

	textAddr.setFont(game_resources->larabiefont_monospace);
	textAddr.setCharacterSize(25);
	textAddr.setString(addr);

	textAnchorPos.setFont(game_resources->larabiefont_monospace);
	textAnchorPos.setCharacterSize(25);
	textAnchorPos.setString("[-]");

	clockLastSeen.restart();
	nextRangePos = 0;
	nextRangePosCalib = 0;
	this->addr = addr;
	this->range[nextRangePos++] = length;
	this->circleMeanValue.setPosition(game_settings->windowX / 2, game_settings->windowY / 2);
	this->circleShadow.setPosition(game_settings->windowX / 2, game_settings->windowY / 2);
	this->circleLast.setPosition(game_settings->windowX / 2, game_settings->windowY / 2);
	this->dotCircle.setRadius(3);
	for(int i = 0; i < 10; i++){
		this->range[i] = length;
		this->rangeCalib[i] = length;


	}




}

void Device::newRange(float length) {
	clockLastSeen.restart();
	if(nextRangePos >9){
		nextRangePos = 0;
	}
	this->range[nextRangePos++] = length;
}

float Device::getMeanRange() {
	float meanRange = 0;
	for(int i = 0; i < 10; i++){
		meanRange += range[i];
	}

	return meanRange/10;
}

void Device::newCalibRange(float length) {
	clockLastSeen.restart();
	if(nextRangePosCalib > 9){
		nextRangePosCalib = 0;
	}
	this->rangeCalib[nextRangePosCalib++] = length;
}

float Device::getMeanCalibRange() {
	float meanRange = 0;
	for(int i = 0; i < 10; i++){
		meanRange += rangeCalib[i];
	}

	return meanRange/10.f;
}

float Device::getLastRange() {
	return this->range[(nextRangePos==0) ? 9 : nextRangePos-1];
}


float Device::getLastCalibRange() {
	return this->rangeCalib[(nextRangePosCalib==0) ? 9 : nextRangePosCalib-1];
}

float Device::getCalibRangeHighest() {
	float highRange = 0;
	for(int i = 0; i <= 9; i++){
		if(rangeCalib[i] > highRange){
			highRange = rangeCalib[i];
		}
	}
	return  highRange;
}

float Device::getCalibRangeLowest() {
	float lowRange = 1000;
	for(int i = 0; i <= 9; i++){
		if(rangeCalib[i] < lowRange){
			lowRange = rangeCalib[i];
		}
	}
	return lowRange;
}

sf::Time Device::getLastSeen() {
	return clockLastSeen.getElapsedTime();
}

void Device::DrawSelf(sf::RenderWindow &window) {

	float radius;
	this->deviceColor = deviceColorarray[deviceNumber];
	textAddr.setColor(this->deviceColor);
	textAddr.setPosition(60, 20 + (30 * deviceNumber));

	if(textAnchorPosSelected){
		sf::Color tmpColor = this->deviceColor;
		if(tmpColor.r == 0){
			tmpColor.r = 50;
		}
		if(tmpColor.g == 0){
			tmpColor.g = 50;
		}
		if(tmpColor.b == 0){
			tmpColor.b = 50;
		}
		textAnchorPos.setColor(sf::Color::Yellow);
	}else{
		textAnchorPos.setColor(this->deviceColor);
	}
	textAnchorPos.setPosition(10, 20 + (30 * deviceNumber));

	float meanRange = 0;
	for (int i = 0; i <= 9; i++) {
		meanRange += rangeCalib[i];
	}
	radius = (meanRange / 10.f) * 100.f;
	char cStr[20];
	sprintf(cStr, "%2.2f", meanRange / 10.f);
	std::string str(cStr);

	sprintf(cStr, "%2.2f", temp);
	std::string str2(cStr);
	textAddr.setString(addr + " dist=" + str + " T=" + str2 + "*C");
	float circle2Radius = getCalibRangeLowest() * 100;
	//radius = getLastCalibRange()*100.f;
	circleMeanValue.setFillColor(sf::Color(0, 0, 0, 0));
	circleLast.setFillColor(sf::Color(0, 0, 0, 0));
	circleShadow.setFillColor(sf::Color(0, 0, 0, 0));

	circleMeanValue.setRadius(radius);
	circleLast.setRadius(getLastCalibRange()*100.f);
	circleShadow.setRadius(circle2Radius);

	circleMeanValue.setPointCount(200);
	circleLast.setPointCount(200);
	circleShadow.setPointCount(200);


	circleMeanValue.setOutlineColor(this->deviceColor);
	circleLast.setOutlineColor(this->deviceColor);
	sf::Color tmpColor = this->deviceColor;
	tmpColor.a = 20;
	circleShadow.setOutlineColor(tmpColor);

	circleMeanValue.setOutlineThickness(3.f);
	circleLast.setOutlineThickness(1.f);
	circleShadow.setOutlineThickness((getCalibRangeHighest() * 100) - circle2Radius);

	if (game_settings->mode_2d == mode_2d_radius_distance) {


		circleMeanValue.setPosition((game_settings->windowX / 2) - (radius), (game_settings->windowY / 2) - (radius));
		circleLast.setPosition((game_settings->windowX / 2) - (getLastCalibRange()*100.f), (game_settings->windowY / 2) - (getLastCalibRange()*100.f));

		circleShadow.setPosition((game_settings->windowX / 2) - (circle2Radius),
							(game_settings->windowY / 2) - (circle2Radius));

		if(radius>0) {
			window.draw(circleMeanValue);
			window.draw(circleLast);
			window.draw(circleShadow);
		}

	} else if(game_settings->mode_2d == mode_2d_4_anchor_coord){
		dotCircle.setFillColor(this->deviceColor);

		radius = (meanRange / 10.f) * (600/(1.20f));
		circleMeanValue.setRadius(radius);
		circle2Radius = getCalibRangeLowest() * (600.f/(1.20f));
		circleShadow.setRadius(circle2Radius);
		circleShadow.setOutlineThickness((getCalibRangeHighest() * (600.f / (1.20f))) - circle2Radius);
		switch (anchorPos){
			case(anchor_pos_none):
				break;
			case anchor_pos_a:
				circleMeanValue.setPosition(((game_settings->windowX / 2) - 300) - (radius), ((game_settings->windowY / 2) - 300) - (radius));
				circleShadow.setPosition(((game_settings->windowX / 2) - 300) - (circle2Radius), ((game_settings->windowY / 2) - 300) - (circle2Radius));
				dotCircle.setPosition(((game_settings->windowX / 2) - 300)-3, ((game_settings->windowY / 2) - 300)-3);
				break;
			case anchor_pos_b:
				circleMeanValue.setPosition((game_settings->windowX / 2) - (radius) + 300, (game_settings->windowY / 2) - (radius) - 300);
				circleShadow.setPosition((game_settings->windowX / 2) - (circle2Radius) + 300, (game_settings->windowY / 2) - (circle2Radius) - 300);
				dotCircle.setPosition(((game_settings->windowX / 2) + 300)-3, ((game_settings->windowY / 2) - 300)-3);
				break;
			case anchor_pos_c:
				circleMeanValue.setPosition((game_settings->windowX / 2) - (radius) + 300, (game_settings->windowY / 2) - (radius) + 300);
				circleShadow.setPosition((game_settings->windowX / 2) - (circle2Radius) + 300, (game_settings->windowY / 2) - (circle2Radius) + 300);
				dotCircle.setPosition(((game_settings->windowX / 2) + 300)-3, ((game_settings->windowY / 2) + 300)-3);
				break;
			case anchor_pos_d:
				circleMeanValue.setPosition((game_settings->windowX / 2) - (radius) - 300, (game_settings->windowY / 2) - (radius) + 300);
				circleShadow.setPosition((game_settings->windowX / 2) - (circle2Radius) - 300, (game_settings->windowY / 2) - (circle2Radius) + 300);
				dotCircle.setPosition(((game_settings->windowX / 2) - 300)-3, ((game_settings->windowY / 2) + 300)-3);
				break;
		}
		if((anchorPos != anchor_pos_none) && (radius>0)){
			window.draw(dotCircle);
			window.draw(circleMeanValue);
			window.draw(circleShadow);
		}

	}
	switch (anchorPos){
		case anchor_pos_a:		textAnchorPos.setString("[a]"); break;
		case anchor_pos_b:		textAnchorPos.setString("[b]"); break;
		case anchor_pos_c:		textAnchorPos.setString("[c]"); break;
		case anchor_pos_d:		textAnchorPos.setString("[d]"); break;
		case(anchor_pos_none):	textAnchorPos.setString("[-]"); break;
	}



	window.draw(textAddr);
	window.draw(textAnchorPos);

}

Device::~Device() {
			switch (anchorPos){
			case(anchor_pos_none):
				break;
			case anchor_pos_a:
				game_settings->anchor_pos_a_taken = false;
				break;
			case anchor_pos_b:
				game_settings->anchor_pos_b_taken = false;
				break;
			case anchor_pos_c:
				game_settings->anchor_pos_c_taken = false;
				break;
			case anchor_pos_d:
				game_settings->anchor_pos_d_taken = false;
				break;
		}
}
