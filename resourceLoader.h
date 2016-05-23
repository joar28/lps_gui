//
// Created by joar on 1/2/16.
//

#ifndef BF_RECOURSELOADER_H
#define BF_RECOURSELOADER_H

#include <map>
#include <SFML/Graphics.hpp>

class resourceLoader {
public:
	resourceLoader();
	sf::Font playfair_regular;
	sf::Font larabiefont_monospace;
private:
	void loadFonts();
};


#endif //BF_RECOURSELOADER_H
