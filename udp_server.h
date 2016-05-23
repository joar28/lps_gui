//
// Created by joar on 5/18/16.
//

#ifndef UART_GUI_UDP_SERVER_H
#define UART_GUI_UDP_SERVER_H

#include <set>
#include "settings.h"
#include "Device.h"


void udpServer(settings *udp_game_settings, std::set<Device*> *devices);


#endif //UART_GUI_UDP_SERVER_H
