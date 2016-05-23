//
// Created by joar on 5/9/16.
//

#include <iostream>
#include "state2D.h"
#include "../Device.h"
#include "../udp_server.h"
#include <unistd.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <fcntl.h>			//Used for UART
#include <string.h>
#include <algorithm>
#include <iomanip>
#include <Eigen/Dense>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

const int x = 0;
const int y = 1;
const int z = 2;
float coX, coY, coZ;

Eigen::MatrixXd matX(3,3);
Eigen::MatrixXd matY(3,3);
Eigen::MatrixXd matZ(3,3);
Eigen::MatrixXd matNe(3,3);

float tagTemp;
int printer = 10;

int uart0_filestream = -1;
struct termios old_options;


struct find_by_address{
	find_by_address(const std::string & address) : address(address) {}
	bool operator()(const Device & device) {
		return device.addr == address;
	}
private:
	std::string address;
};


state2D::state2D(settings *settings, resourceLoader *res) {
	this->game_settings = settings;
	this->game_resources = res;

	this->game_settings->udp_server_active = true;

//	std::thread udpServerThread(udpServer, this->game_settings, &devices);

	udpServerThread = new std::thread(udpServer, this->game_settings, &devices);

	this->tagPos.setRadius(4);
	anchorDistance = 1.8;

	textMode.setFont(game_resources->larabiefont_monospace);
	textMode.setCharacterSize(25);
	textMode.setString("lololo");
	textMode.setPosition(game_settings->windowX-250, 20);

	textIpEth.setFont(game_resources->larabiefont_monospace);
	textIpEth.setCharacterSize(20);
	textIpEth.setString("");
	textIpEth.setPosition(game_settings->windowX-255, game_settings->windowY-45);
	textIpWlan.setFont(game_resources->larabiefont_monospace);
	textIpWlan.setCharacterSize(20);
	textIpWlan.setString("");
	textIpWlan.setPosition(game_settings->windowX-255, game_settings->windowY-25);

	textCoordinat.setFont(game_resources->larabiefont_monospace);
	textCoordinat.setCharacterSize(25);
	textCoordinat.setPosition(25, game_settings->windowY - 50);

	this->stateNext = this;
	uart0_filestream = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		std::cout << "Error - Unable to open UART.  Ensure it is not in use by another application" << std::endl;
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream,&old_options); /* save current serial port settings */
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);	//-------------------------

}

void state2D::EventKeyboard(sf::Event event) {
	if(event.KeyPressed){
		if(event.key.code == sf::Keyboard::Escape){
			this->stateNext = nullptr;
		}
	}
}

void state2D::EventMouse(sf::Event event) {
	switch (event.type){
		case sf::Event::MouseButtonPressed:  {

			float textLeft = this->textMode.getGlobalBounds().left;
			float textRight = textLeft + this->textMode.getGlobalBounds().width;
			float textTop = this->textMode.getGlobalBounds().top;
			float textBottom = textTop + this->textMode.getGlobalBounds().height;
			if ((textLeft <= sf::Mouse::getPosition(*game_settings->window).x && textRight >= sf::Mouse::getPosition(*game_settings->window).x) &&
				(textTop <= sf::Mouse::getPosition(*game_settings->window).y && textBottom >= sf::Mouse::getPosition(*game_settings->window).y))
			{
				if(game_settings->mode_2d == mode_2d_radius_distance){
					game_settings->mode_2d = mode_2d_4_anchor_coord;
				} else if(game_settings->mode_2d == mode_2d_4_anchor_coord){
					game_settings->mode_2d = mode_2d_radius_distance;
				}
			}

			Device *tempDev = NULL;
			int i = 0;
			std::set<Device *>::iterator it;
			for (it = devices.begin(); it != devices.end(); ++it) {
				tempDev = *it;
//				tempDev->deviceNumber = i++;
//				tempDev->DrawSelf(window);

				textLeft = tempDev->textAnchorPos.getGlobalBounds().left;
				textRight = textLeft + tempDev->textAnchorPos.getGlobalBounds().width;
				textTop = tempDev->textAnchorPos.getGlobalBounds().top;
				textBottom = textTop + tempDev->textAnchorPos.getGlobalBounds().height;

//				if ((textLeft <= sf::Mouse::getPosition(*windoww).x && textRight  >= sf::Mouse::getPosition(*windoww).x) &&
//				   (textTop   <= sf::Mouse::getPosition(*windoww).y && textBottom >= sf::Mouse::getPosition(*windoww).y)){


				if ((textLeft <= sf::Mouse::getPosition(*game_settings->window).x && textRight >= sf::Mouse::getPosition(*game_settings->window).x) &&
					(textTop <= sf::Mouse::getPosition(*game_settings->window).y && textBottom >= sf::Mouse::getPosition(*game_settings->window).y))
				{
					tempDev->textAnchorPosSelected = true;
					if(tempDev->anchorPos == anchor_pos_none){
						if(!game_settings->anchor_pos_a_taken) {
							tempDev->anchorPos = anchor_pos_a;
							game_settings->anchor_pos_a_taken = true;
						}else if(!game_settings->anchor_pos_b_taken){
							tempDev->anchorPos = anchor_pos_b;
							game_settings->anchor_pos_b_taken = true;
						}else if(!game_settings->anchor_pos_c_taken){
							tempDev->anchorPos = anchor_pos_c;
							game_settings->anchor_pos_c_taken = true;
						}else if(!game_settings->anchor_pos_d_taken) {
							tempDev->anchorPos = anchor_pos_d;
							game_settings->anchor_pos_d_taken = true;
						}else{
							tempDev->anchorPos = anchor_pos_none;
						}
					} else if(tempDev->anchorPos == anchor_pos_a){
						if(!game_settings->anchor_pos_b_taken){
							tempDev->anchorPos = anchor_pos_b;
							game_settings->anchor_pos_b_taken = true;
							game_settings->anchor_pos_a_taken = false;
						}else if(!game_settings->anchor_pos_c_taken){
							tempDev->anchorPos = anchor_pos_c;
							game_settings->anchor_pos_c_taken = true;
							game_settings->anchor_pos_a_taken = false;
						}else if(!game_settings->anchor_pos_d_taken) {
							tempDev->anchorPos = anchor_pos_d;
							game_settings->anchor_pos_d_taken = true;
							game_settings->anchor_pos_a_taken = false;
						}else{
							tempDev->anchorPos = anchor_pos_none;
							game_settings->anchor_pos_a_taken = false;
						}
					} else if(tempDev->anchorPos == anchor_pos_b){
						if(!game_settings->anchor_pos_c_taken){
							tempDev->anchorPos = anchor_pos_c;
							game_settings->anchor_pos_c_taken = true;
							game_settings->anchor_pos_b_taken = false;
						}else if(!game_settings->anchor_pos_d_taken) {
							tempDev->anchorPos = anchor_pos_d;
							game_settings->anchor_pos_d_taken = true;
							game_settings->anchor_pos_b_taken = false;
						}else{
							tempDev->anchorPos = anchor_pos_none;
							game_settings->anchor_pos_b_taken = false;
						}
					} else if(tempDev->anchorPos == anchor_pos_c){
						if(!game_settings->anchor_pos_d_taken) {
							tempDev->anchorPos = anchor_pos_d;
							game_settings->anchor_pos_d_taken = true;
							game_settings->anchor_pos_c_taken = false;
						}else{
							tempDev->anchorPos = anchor_pos_none;
							game_settings->anchor_pos_c_taken = false;
						}
					} else {
						tempDev->anchorPos = anchor_pos_none;
						game_settings->anchor_pos_d_taken = false;
					}
				}
			}
		}
			break;
		case sf::Event::MouseButtonReleased:
			break;
		case sf::Event::MouseEntered:
			break;
		case sf::Event::MouseLeft:
			break;
		case sf::Event::MouseMoved: {
			Device *tempDev = NULL;
			int i = 0;
			std::set<Device *>::iterator it;
			for (it = devices.begin(); it != devices.end(); ++it) {
				tempDev = *it;
//				tempDev->deviceNumber = i++;
//				tempDev->DrawSelf(window);

				float textLeft = tempDev->textAnchorPos.getGlobalBounds().left;
				float textRight = textLeft + tempDev->textAnchorPos.getGlobalBounds().width;
				float textTop = tempDev->textAnchorPos.getGlobalBounds().top;
				float textBottom = textTop + tempDev->textAnchorPos.getGlobalBounds().height;

				if ((textLeft <= event.mouseMove.x && textRight >= event.mouseMove.x) &&
					(textTop <= event.mouseMove.y && textBottom >= event.mouseMove.y)) {
//					tempDev->textAnchorPos.setCharacterSize(50);
//					tempDev->textAnchorPos.setColor(sf::Color::Yellow);
					tempDev->textAnchorPosSelected = true;
//					tempDev->textAnchorPos.setPosition(50,120+(50*i));
				} else {
//					tempDev->textAnchorPos.setCharacterSize(35);
//					tempDev->textAnchorPos.setColor(sf::Color::White);
					tempDev->textAnchorPosSelected = false;
					//tempDev->textAnchorPos.setPosition(50,130+(50*i));
				}
			}
		}
			break;
		case sf::Event::MouseWheelMoved:
			break;
		default:
			break;
	}
}


void state2D::HasTimedOut() {
	Device *tempDev = NULL;
	std::set<Device *>::iterator it;
	for (it = devices.begin(); it != devices.end(); ++it) {
		tempDev = *it;
		if (tempDev->getLastSeen().asSeconds() >= 5.f) {
			delete(tempDev);
			devices.erase (it);
		}

	}
}

void state2D::Process(sf::Time TimeSinceLastTick) {

	//usleep(1000); // takes microseconds
	HasTimedOut();
	//----- CHECK FOR ANY RX BYTES -----
	if (uart0_filestream != -1) {
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[256];
		int rx_length = (int) read(uart0_filestream, (void *) rx_buffer, 255);        //Filestream, buffer to store in, number of bytes to read (max)

		if (rx_length < 0) {
			//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0) {
			//No data waiting
		}
		else {
			//Bytes received
			rx_buffer[rx_length] = '\0';
			//	printf("%i bytes read : %s\n", rx_length, rx_buffer);
			if (strncmp((char *) rx_buffer, "RNG", 3) == 0) {
				printer++;
				//char * strtok ( char * str, const char * delimiters );
				char *delimited;
				delimited = strtok((char *) rx_buffer, ",");
				int i = 0;

				Device *tempDev = NULL;
				bool alreadyExists = false;
				std::string tempAddr;
				float tempRange;
				std::set<Device>::iterator result;
				while (delimited != NULL) {
					if (i == 1) {
						std::string taddr(&delimited[2]);
						tempAddr = taddr;

						std::set<Device *>::iterator it;
						for (it = devices.begin(); it != devices.end(); ++it) {
							tempDev = *it;
							if (tempDev->addr == tempAddr) {
								alreadyExists = true;
								break;
							}
						}


					}

					if (i == 2) {
						std::string str(delimited);
						if (alreadyExists) {
							tempDev->newRange(std::stof(str));
						}
						tempRange = std::stof(str);
					} else if (i == 3) {
						std::string str(delimited);
						if (alreadyExists) {
							tempDev->temp = std::stof(str);
						}
					} else if (i == 4) {
						std::string str(delimited);
						tagTemp = std::stof(str);
					} else if (i == 9) {
						std::string str(delimited);
						if (alreadyExists) {
							tempDev->newCalibRange(std::stof(str));
						}
						tempRange = std::stof(str);
					}

					i++;
					delimited = strtok(NULL, ",");
				}
				if (alreadyExists) {
//					std::cout << "Already exists" << std::endl;
				} else {
//					std::cout << "New device" << std::endl;
					tempDev = new Device(tempAddr, tempRange, game_settings, game_resources);
					devices.insert(tempDev);
				}

				std::string rx_string((char *) rx_buffer);
				//std::cout << rx_string << std::endl;
			}
//			printf("%s\n", rx_buffer);
		}

	}

	if (printer > 10) {
		printer = 0;

		struct ifaddrs *ifap, *ifa;
		struct sockaddr_in *sa;
		char *addr;

		getifaddrs (&ifap);
		bool wlanExist, ethExist = false;
		for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr->sa_family==AF_INET) {
				sa = (struct sockaddr_in *) ifa->ifa_addr;
				addr = inet_ntoa(sa->sin_addr);
				printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);

				if (strcmp(ifa->ifa_name, "eth0") == 0){
					ethExist = true;
					std::string strIp(addr);
					textIpEth.setString("eth0: " + strIp);
				} else if (strcmp(ifa->ifa_name, "wlan0") == 0){
					wlanExist = true;
					std::string strIp(addr);
					textIpWlan.setString("Wlan: " + strIp);
				}

			}
		}
		if(!wlanExist){
			textIpWlan.setString("");
		}
		if(!ethExist){
			textIpEth.setString("");
		}
		freeifaddrs(ifap);


		std::set<Device *>::iterator it;
		for (it = devices.begin(); it != devices.end(); ++it) {
			Device *tempDev = *it;
			std::cout << "0x" << tempDev->addr
			<< " Rng=" << std::fixed << std::setprecision(2) << tempDev->getMeanRange()
			<< " Cng=" << std::fixed << std::setprecision(2) << tempDev->getMeanCalibRange()
			<< " TmpA=" << std::fixed << std::setprecision(2) << tempDev->temp
			<< " TmpT=" << std::fixed << std::setprecision(2) << tagTemp
			<< " LastSeen=" << std::fixed << std::setprecision(2) << tempDev->getLastSeen().asSeconds()
			<< "\n";
		}
		std::cout << std::endl;
	}

	if(game_settings->mode_2d == mode_2d_radius_distance){
		textMode.setString("Mode: Tag radius");
	}else if (game_settings->mode_2d == mode_2d_4_anchor_coord){
		textMode.setString("Mode: 4 anchor");
		if(game_settings->anchor_pos_a_taken && game_settings->anchor_pos_b_taken && game_settings->anchor_pos_c_taken ){

			float lenA, lenB, lenC, lenD = 0;
			Device *tempDev = NULL;
			int i = 0;
			std::set<Device *>::iterator it;
			for (it = devices.begin(); it != devices.end(); ++it) {
				tempDev = *it;
				if(tempDev->anchorPos == anchor_pos_a){
					lenA = tempDev->getMeanCalibRange();
					lenA = tempDev->getLastCalibRange();
				} else if(tempDev->anchorPos == anchor_pos_b){
					lenB = tempDev->getMeanCalibRange();
//					lenB = tempDev->getLastCalibRange();
				} else if(tempDev->anchorPos == anchor_pos_c){
					lenC = tempDev->getMeanCalibRange();
//					lenC = tempDev->getLastCalibRange();
				} else if(tempDev->anchorPos == anchor_pos_d){
					lenD = tempDev->getMeanCalibRange();
//					lenD = tempDev->getLastCalibRange();
				}

			}

			bool oldTlat = false;

				float a[3] = {0, 				0,				0};
				float b[3] = {anchorDistance,	0,				0};
				float c[3] = {anchorDistance,	anchorDistance, 0};
				float d[3] = {0,				anchorDistance, 0};
			if(oldTlat) {

				float a1, a2, a3, b1, b2, b3, c1, c2, c3, d1, d2, d3;

				a1 = b[x] - a[x];
				b1 = b[y] - a[y];
				c1 = b[z] - a[z];

				d1 = 0.5f *
					 ((lenA * lenA) - (lenB * lenB) + (b[x] * b[x]) - (a[x] * a[x]) + (b[y] * b[y]) - (a[y] * a[y]) +
					  (b[z] * b[z]) - (a[z] * a[z]));


				a2 = c[x] - a[x];
				b2 = c[y] - a[y];
				c2 = c[z] - a[z];

				d2 = 0.5f *
					 ((lenA * lenA) - (lenC * lenC) + (c[x] * c[x]) - (a[x] * a[x]) + (c[y] * c[y]) - (a[y] * a[y]) +
					  (c[z] * c[z]) - (a[z] * a[z]));

				a3 = d[x] - a[x];
				b3 = d[y] - a[y];
				c3 = d[z] - a[z];

				d3 = 0.5f *
					 ((lenA * lenA) - (lenD * lenD) + (d[x] * d[x]) - (a[x] * a[x]) + (d[y] * d[y]) - (a[y] * a[y]) +
					  (d[z] * d[z]) - (a[z] * a[z]));

				matNe(0, 0) = a1;
				matNe(1, 0) = a2;
				matNe(2, 0) = a3;
				matNe(0, 1) = b1;
				matNe(1, 1) = b2;
				matNe(2, 1) = b3;
				matNe(0, 2) = c1;
				matNe(1, 2) = c2;
				matNe(2, 2) = c3;

				matX(0, 0) = d1;
				matX(1, 0) = d2;
				matX(2, 0) = d3;
				matX(0, 1) = b1;
				matX(1, 1) = b2;
				matX(2, 1) = b3;
				matX(0, 2) = c1;
				matX(1, 2) = c2;
				matX(2, 2) = c3;

				matY(0, 0) = a1;
				matY(1, 0) = a2;
				matY(2, 0) = a3;
				matY(0, 1) = d1;
				matY(1, 1) = d2;
				matY(2, 1) = d3;
				matY(0, 2) = c1;
				matY(1, 2) = c2;
				matY(2, 2) = c3;

				matZ(0, 0) = a1;
				matZ(1, 0) = a2;
				matZ(2, 0) = a3;
				matZ(0, 1) = b1;
				matZ(1, 1) = b2;
				matZ(2, 1) = b3;
				matZ(0, 2) = d1;
				matZ(1, 2) = d2;
				matZ(2, 2) = d3;

				coX = (float) (matX.determinant() / matNe.determinant());
				coY = (float) (matY.determinant() / matNe.determinant());
				coZ = (float) (matZ.determinant() / matNe.determinant());

				std::cout << "koordinat = "
				<< " X=" << std::fixed << std::setprecision(2) << coX
				<< " Y=" << std::fixed << std::setprecision(2) << coY
				<< " Z=" << std::fixed << std::setprecision(2) << coZ
				<< std::endl;

				char cStr[20];
				sprintf(cStr, "%2.4f", coX);
				std::string strX(cStr);
				sprintf(cStr, "%2.4f", coY);
				std::string strY(cStr);
				sprintf(cStr, "%2.4f", coZ);
				std::string strZ(cStr);

				textCoordinat.setString("X=" + strX + "  Y=" + strY + "  Z=" + strZ);
			}else{


				coX = ((lenA*lenA)-(lenB*lenB)+(b[x] * b[x])) / (2 * b[x]);
				coY = (( (lenA*lenA) - (lenC*lenC) + (c[x] * c[x]) + (c[y] * c[y]) ) / ( 2 * c[y] ) ) - ( ( c[x] / c[y] ) * coX );
				coZ = sqrt( (lenA*lenA) - (coX * coX) - (coY * coY) );

				char cStr[20];
				sprintf(cStr, "%2.2f", coX);
				std::string strX(cStr);
				sprintf(cStr, "%2.2f", coY);
				std::string strY(cStr);
				sprintf(cStr, "%2.2f", coZ);
				std::string strZ(cStr);

				textCoordinat.setString("X=" + strX + "  Y=" + strY + "  Z=" + strZ);

				this->tagPos.setPosition((game_settings->windowX/2) - 300.f + ((600.f / anchorDistance) * (float)coX), ((game_settings->windowY/2) - 300.f + ((600.f / anchorDistance) * (float)coY)));
			}

		}
	}



}

void state2D::Draw(sf::RenderWindow &window) {
	window.draw(textMode);
	window.draw(textIpEth);
	window.draw(textIpWlan);
	Device *tempDev = NULL;
	int i = 0;
	std::set<Device *>::iterator it;
	for (it = devices.begin(); it != devices.end(); ++it) {
		tempDev = *it;
		tempDev->deviceNumber = i++;
		tempDev->DrawSelf(window);

	}
	if(game_settings->anchor_pos_a_taken && game_settings->anchor_pos_b_taken && game_settings->anchor_pos_c_taken ){
		window.draw(textCoordinat);
	}
	if(game_settings->mode_2d == mode_2d_4_anchor_coord){
		window.draw(tagPos);
	}
}

baseState *state2D::NextState() {
	return this->stateNext;
}

state2D::~state2D() {

	//----- CLOSE THE UART -----
	close(uart0_filestream);
	/* restore the old port settings */
	tcsetattr(uart0_filestream,TCSANOW,&old_options);
	std::cout << "Close UART" << std::endl;
}
