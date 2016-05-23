//
// Created by joar on 1/2/16.
//

#include "resourceLoader.h"
#include <iostream>
resourceLoader::resourceLoader() {
	loadFonts();

}

void resourceLoader::loadFonts() {
	std::string path = "recourses/fonts/playfair_display/PlayfairDisplay-Regular.ttf";
	if (!playfair_regular.loadFromFile(path))
		std::cout << "Error! RecourceLoader: " << path << std::endl;

		path = "recourses/fonts/larabiefont_rg.ttf";
	if (!larabiefont_monospace.loadFromFile(path))
		std::cout << "Error! RecourceLoader: " << path << std::endl;


}
