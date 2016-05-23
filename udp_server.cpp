//
// Created by joar on 5/18/16.
//
/* udpserver.c
 *
 * Linux test commando
 * ncat -vv localhost 8999 -u
 * ncat -v localhost 8999 -u
 *
 * */

#include "udp_server.h"
#include "Device.h"
#include <iostream>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iomanip>


void udpServer(settings *udp_game_settings, std::set<Device*> *devices) {


	int sock;
	socklen_t addr_len;
	ssize_t bytes_read;
	char recv_data[1024];
	struct sockaddr_in server_addr , client_addr;


	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	uint16_t recievPortNumber = 8999;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(recievPortNumber);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);


	if (bind(sock,(struct sockaddr *)&server_addr,
			 sizeof(struct sockaddr)) == -1)
	{
		perror("Bind");
		exit(1);
	}

	addr_len = sizeof(struct sockaddr);

	std::cout << "\n" << "UDPServer Waiting for client on port " << recievPortNumber << std::endl;
	fflush(stdout);

//	while (udp_game_settings->udp_server_active)
	while (true)
	{

		bytes_read = recvfrom(sock, recv_data, 1024,0, (struct sockaddr *)&client_addr, &addr_len);
		recv_data[bytes_read] = '\0';

		std::cout << inet_ntoa(client_addr.sin_addr) << " , " << ntohs(client_addr.sin_port) << " said : " << recv_data << std::endl;

//		float* floatRead = (float*)recv_data;
//		float floatWrite[4]; // No. of floats to send to labview
//		for (int i = 0; i < 4; i++) {
//			floatWrite[i] = (float)floatRead[i]*2.0f;
//		}
//		char* bufferWrite = (char*)floatWrite;
		char bufferWrite[1024];
		strcpy(bufferWrite, recv_data);


		if(strcmp(recv_data, "anchor\n") == 0){
			std::string outStr;
			std::set<Device *>::iterator it;
			for (it = devices->begin(); it != devices->end(); ++it) {
				Device *tempDev = *it;
				std::cout << "0x" << tempDev->addr
				<< " Rng=" << std::fixed << std::setprecision(2) << tempDev->getMeanRange()
				<< " Cng=" << std::fixed << std::setprecision(2) << tempDev->getMeanCalibRange()
				<< " TmpA=" << std::fixed << std::setprecision(2) << tempDev->temp
//				<< " TmpT=" << std::fixed << std::setprecision(2) << tagTemp
				<< " LastSeen=" << std::fixed << std::setprecision(2) << tempDev->getLastSeen().asSeconds()
				<< "\n";

				outStr += "0x" +  tempDev->addr +  " Rng=" + std::to_string(tempDev->getMeanCalibRange()) + "\n";
			}
			std::cout << std::endl;

			//bufferWrite = (char *) malloc(sizeof(outStr.c_str())*sizeof(char));
			strcpy(bufferWrite, outStr.c_str());

		}


		size_t aa = strlen(bufferWrite);

		if (sendto(sock, bufferWrite, aa, 0, (struct sockaddr*) &client_addr, addr_len) == -1){
			//printf("sendto() failed with error code : %d" , WSAGetLastError());
			std::cout << "sendto() failed with error code : xxxxx" << std::endl;
			exit(EXIT_FAILURE);
		}

	}








	std::cout << "UDP server thread closing" << std::endl;
}
