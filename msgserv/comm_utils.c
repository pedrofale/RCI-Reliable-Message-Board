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

 int SOCKET_have_same_addr(SOCKET *sckt1, SOCKET *sckt2) {
 	int have_same_addr = 0;
 	if(sckt1->addr.sin_port == sckt2->addr.sin_port)
 		if(sckt1->addr.sin_addr.s_addr == sckt2->addr.sin_addr.s_addr)
 			have_same_addr = 1;

 	return have_same_addr;
 }

 int SOCKET_close(SOCKET *sckt) {
 	int err = 0;
 	if(sckt != NULL) {
	 	close(sckt->fd);
	 	free(sckt);
	 }
 }


/****************************************************
 * UDP methods
 ****************************************************/

 // create UDP sckt as a client of the server at ip
 SOCKET* create_udp_client_socket(struct in_addr ip, int port) {
 	SOCKET* sckt;

 	// allocate memory for the SOCKET structure
 	if((sckt = malloc(sizeof(SOCKET))) == NULL) {
 		fprintf(stderr, "error: malloc() failed\n");
 		return NULL;
 	} 

	// create an endpoint for communication via UDP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		free(sckt);
		return NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		fprintf(stderr, "error: memset() failed\n");
		SOCKET_close(sckt);
		return NULL;
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
 	if((sckt = malloc(sizeof(SOCKET))) == NULL) {
 		fprintf(stderr, "error: malloc() failed\n");
 		return NULL;
 	} 

	// create an endpoint for communication via UDP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		free(sckt);
		return NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		fprintf(stderr, "error: memset() failed\n");
		SOCKET_close(sckt);
		return NULL;
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sckt->fd, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr)) == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));
		SOCKET_close(sckt);
		return NULL;
	}
	
	return sckt;
 }


 // write to the sckt
 int sendmsg_udp(SOCKET *sckt, char *msg, int msglen) {
 	int err = 0;
 	int n = 0;
 	if(sendto(sckt->fd, msg, msglen, 0, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		err = -1;
	}

	return err;
 }

 // read from the sckt
 int readmsg_udp(SOCKET *sckt, char *msg, int msglen) {
 	int err = 0;
 	int addrlen = sizeof(sckt->addr);

 	if(recvfrom(sckt->fd, msg, msglen, 0, (struct sockaddr*)&sckt->addr, &addrlen) == -1) {
		fprintf(stderr, "error: %s\n",strerror(errno));
		err = -1;
	}

	return err;
 }


/****************************************************
 * TCP methods
 ****************************************************/

 // create TCP sckt as a client of the server at ip
 SOCKET* create_tcp_client_socket(struct in_addr ip, int port) {
 	SOCKET* sckt;

 	// allocate memory for the SOCKET structure
 	if((sckt = malloc(sizeof(SOCKET))) == NULL) {
 		fprintf(stderr, "error: malloc() failed\n");
 		return NULL;
 	} 

	// create an endpoint for communication via TCP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		free(sckt);
		return NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		fprintf(stderr, "error: memset() failed\n");
		SOCKET_close(sckt);
		return NULL;
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = ip.s_addr;

	if(connect(sckt->fd, (struct sockaddr*)&sckt->addr,sizeof(sckt->addr)) == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));
		SOCKET_close(sckt);
		return NULL;
	}

	return sckt;
 } 

 // create TCP sckt as a server
 SOCKET* create_tcp_server_socket(int port) {
 	SOCKET* sckt;

 	// allocate memory for the SOCKET structure
 	if((sckt = malloc(sizeof(SOCKET))) == NULL) {
 		fprintf(stderr, "error: malloc() failed\n");
 		return NULL;
 	} 

	// create an endpoint for communication via TCP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		free(sckt);
		return NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		fprintf(stderr, "error: memset() failed\n");
		SOCKET_close(sckt);
		return NULL;
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sckt->fd, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr)) == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));
		SOCKET_close(sckt);
		return NULL;
	}

	if(listen(sckt->fd, 5) == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));		
		SOCKET_close(sckt);
		return NULL;	
	}	

	return sckt;
 }


SOCKET* accept_tcp_server_socket(SOCKET *sckt) {
	SOCKET* new_sckt;
	int addrlen = 0;

	if((new_sckt = malloc(sizeof(SOCKET))) == NULL) {
 		fprintf(stderr, "error: malloc() failed\n");
 		return NULL;
 	} 

	//new_sckt->addr.sin_family = sckt->addr.sin_family;
	//new_sckt->addr.sin_port = sckt->addr.sin_port;
	//new_sckt->addr.sin_addr.s_addr = sckt->addr.sin_addr.s_addr;

	addrlen = sizeof(new_sckt->addr);
	
	if((new_sckt->fd = accept(sckt->fd, (struct sockaddr*)&new_sckt->addr, &addrlen)) == -1) {
		fprintf("error: %s\n", strerror(errno));
		SOCKET_close(sckt);		
		return NULL;	
	}

	return new_sckt;
}

 // write to the sckt
 int writemsg_tcp(SOCKET *sckt, char *msgptr, int msglen) {
 	int err = 0;
 	int nbytes, nleft, nwritten;

 	nbytes = msglen;
 	nleft = nbytes;
 	
 	while(nleft > 0) {
 		if((nwritten = write(sckt->fd, msgptr, nleft)) < 0) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			err = -1;
			break;
		}
		nleft = nleft - nwritten;
		msgptr = msgptr + nwritten;
 	}

	return err;
 }

 // read from the sckt
 int readmsg_tcp(SOCKET *sckt, char *msgptr, int msglen) {
 	int err = 1;
 	int nbytes, nleft, nread;

 	nbytes = msglen;
 	nleft = nbytes;

 	while(nleft > 0) {
	 	if((nread = read(sckt->fd, msgptr, nleft)) == -1) {
			fprintf(stderr, "error: %s\n",strerror(errno));
			err = nread;
			break;
		}
		else if(nread == 0) { // closed by peer
			err = nread;
			break;
		}
		nleft = nleft - nread;
		msgptr = msgptr + nread; 
	}

	return err;
 }