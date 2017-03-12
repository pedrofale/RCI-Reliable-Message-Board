/******************************************************************************
 * File Name: comm_utils.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     comm_utils: utilities for communication via UDP and TCP sockets
 *
 * DESCRIPTION
 *      Implementation of methods regarding socket communication via UDP and TCP
 *
 * METHODS
 
 *****************************************************************************/

 #include "comm_utils.h"

 struct sSOCKET {
 	int fd;
 	struct sockaddr_in addr;
 };


 void SOCKET_set_fd(SOCKET *sckt, int fd) {
 	sckt->fd = fd;
 }

 int SOCKET_get_fd(SOCKET *sckt) {
 	return sckt->fd;
 }

 void SOCKET_set_addr(SOCKET *sckt, struct sockaddr_in addr) {
 	sckt->addr.sin_addr = addr.sin_addr;
 }

 struct sockaddr_in SOCKET_get_addr(SOCKET *sckt) {
 	return sckt->addr;
 }

 // create UDP sckt as a client of the server at ip
 SOCKET* create_udp_client_socket(struct in_addr ip, int port) {
 	SOCKET* sckt;

 	// allocate memory for the SOCKET structure
 	sckt = malloc(sizeof(SOCKET));

	// create an endpoint for communication via UDP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("error: %s\n", strerror(errno));
		sckt = NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		printf("error: %s\n", strerror(errno));
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = ip.s_addr;
	
	return sckt;
 } 

 // create UDP sckt as a server
 SOCKET* create_udp_server_socket(int port) {
 	SOCKET* sckt;

 	// allocate memory for the SOCKET structure
 	sckt = malloc(sizeof(SOCKET));

	// create an endpoint for communication via UDP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("error: %s\n", strerror(errno));
		sckt = NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		printf("error: %s\n", strerror(errno));
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sckt->fd, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr));
	
	return sckt;
 }

	
 int close_udp_socket(SOCKET *sckt) {
 	int err = 0;
 	close(sckt);
 	free(sckt);
 }


 // write to the sckt
 int sendmsg_udp(SOCKET *sckt, char *msg, int msglen) {
 	int err = 0;
 	int n = 0;
 	if(sendto(sckt->fd, msg, msglen, 0, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr)) == -1) {
		printf("error: %s\n", strerror(errno));
		err = -1;
	}

	return err;
 }

 // read from the sckt
 int readmsg_udp(SOCKET *sckt, char *msg, int msglen) {
 	int err = 0;
 	int addrlen = sizeof(sckt->addr);

 	if(recvfrom(sckt->fd, msg, msglen, 0, (struct sockaddr*)&sckt->addr,&addrlen) == -1) {
		printf("error: %s\n",strerror(errno));
		err = -1;
	}

	return err;
 }