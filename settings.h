//
// Created by joar on 1/2/16.
//

#ifndef BF_SETTINGS_H
#define BF_SETTINGS_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <Eigen/Dense>

typedef enum {
		mode_2d_radius_distance,
		mode_2d_4_anchor_coord
	}mode_2d_t;

typedef enum {
		anchor_pos_none,
		anchor_pos_a,
		anchor_pos_b,
		anchor_pos_c,
		anchor_pos_d,
	}anchor_pos_t;

class settings {
public:
	settings();
	~settings();
	unsigned int windowX;
	unsigned int windowY;

	mode_2d_t mode_2d = mode_2d_radius_distance;
	bool anchor_pos_a_taken = false;
	bool anchor_pos_b_taken = false;
	bool anchor_pos_c_taken = false;
	bool anchor_pos_d_taken = false;
	sf::RenderWindow *window = nullptr;

	bool udp_server_active = false;
};


#endif //BF_SETTINGS_H
