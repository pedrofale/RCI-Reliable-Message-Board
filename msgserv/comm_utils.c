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


/****************************************************
 * UDP methods
 ****************************************************/

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
 	if(sckt != NULL) {
	 	close(sckt->fd);
	 	free(sckt);
	 }
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

 	if(recvfrom(sckt->fd, msg, msglen, 0, (struct sockaddr*)&sckt->addr, &addrlen) == -1) {
		printf("error: %s\n",strerror(errno));
		err = -1;
	}

	return err;
 }


/****************************************************
 * TCP methods
 ****************************************************/

 // create TCP sckt as a client of the server at ip
 SOCKET* create_tcp_client_socket(struct in_addr ip, int port, int timeout) {
 	SOCKET* sckt;

 	struct timeval Timeout;
    Timeout.tv_sec = timeout;
    Timeout.tv_usec = 0;

 	// allocate memory for the SOCKET structure
 	sckt = malloc(sizeof(SOCKET));

	// create an endpoint for communication via TCP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		sckt = NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		fprintf(stderr, "error: %s\n", strerror(errno));
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = ip.s_addr;

	if(connect(sckt->fd, (struct sockaddr*)&sckt->addr,sizeof(sckt->addr)) == -1){
		fprintf(stderr, "error: %s\n", strerror(errno));
		sckt = NULL;
	}

	// long arg; 
	// // Set non-blocking 
	// arg = fcntl(sckt->fd, F_GETFL, NULL); 
	// arg |= O_NONBLOCK; 
	// fcntl(sckt->fd, F_SETFL, arg); 
	   
 //    if(connect(sckt->fd, (struct sockaddr*)&sckt->addr,sizeof(sckt->addr)) < 0){
 //    	socklen_t lon; 
 //    	fd_set myset;
 //    	int valopt;
	// 	if(errno == EINPROGRESS) {
	// 		FD_ZERO(&myset); 
	//         FD_SET(sckt->fd, &myset); 
	//         if (select(sckt->fd+1, NULL, &myset, NULL, &Timeout) > 0) { 
	//            lon = sizeof(int); 
	//            getsockopt(sckt->fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
	//            if (valopt) { 
	//               fprintf(stderr, "Error in connection() %d - %s\n", valopt, strerror(valopt));
	//               sckt = NULL; 
	//            } 
	//         } 
	//         else { 
	//            fprintf(stderr, "Timeout or error() %d - %s\n", valopt, strerror(valopt));  
	//            sckt = NULL;
	//         } 
	// 	}
	// 	else { 
	//         fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno));  
	//         sckt = NULL;
 //     	}
	// }

	// if(sckt != NULL) {
	// 	arg = fcntl(sckt->fd, F_GETFL, NULL); 
	// 	arg &= (~O_NONBLOCK); 
	//     fcntl(sckt->fd, F_SETFL, arg);
	// }

	return sckt;
 } 

 // create TCP sckt as a server
 SOCKET* create_tcp_server_socket(int port) {
 	SOCKET* sckt;
 	SOCKET* new_sckt;

 	// allocate memory for the SOCKET structure
 	sckt = malloc(sizeof(SOCKET));

	// create an endpoint for communication via TCP using IPv4
	if((sckt->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("error: %s\n", strerror(errno));
		sckt = NULL;
	}

	// fill the memory area pointed by the structure addr with \0
	if(memset((void*)&sckt->addr, (int)'\0', sizeof(sckt->addr)) == NULL) {
		printf("error: %s\n", strerror(errno));
		sckt = NULL;
	}

	sckt->addr.sin_family = AF_INET; // IPv4
	sckt->addr.sin_port = htons((u_short)port);
	sckt->addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sckt->fd, (struct sockaddr*)&sckt->addr, sizeof(sckt->addr)) == -1){
		printf("error: %s\n", strerror(errno));
		sckt = NULL;
		return sckt;
	}

	if(listen(sckt->fd, 5) == -1){
		printf("error: %s\n", strerror(errno));		
		sckt = NULL;	
	}	

	return sckt;
 }


SOCKET* accept_tcp_server_socket(SOCKET *sckt) {
	SOCKET* new_sckt;
	int addrlen = 0;

	new_sckt = malloc(sizeof(SOCKET));

	//new_sckt->addr.sin_family = sckt->addr.sin_family;
	//new_sckt->addr.sin_port = sckt->addr.sin_port;
	//new_sckt->addr.sin_addr.s_addr = sckt->addr.sin_addr.s_addr;

	addrlen = sizeof(new_sckt->addr);
	new_sckt->fd = accept(sckt->fd, (struct sockaddr*)&new_sckt->addr, &addrlen);
	if(new_sckt->fd == -1){
		printf("error: %s\n", strerror(errno));			
	}

	return new_sckt;
}
	
 int close_tcp_socket(SOCKET *sckt) {
 	int err = 0;
 	if(sckt != NULL) {
	 	close(sckt->fd);
	 	free(sckt);
	 }
 }


 // write to the sckt
 int writemsg_tcp(SOCKET *sckt, char *msgptr, int msglen) {
 	int err = 0;
 	int nbytes, nleft, nwritten;

 	nbytes = msglen;
 	nleft = nbytes;
 	
 	while(nleft > 0) {
 		if((nwritten = write(sckt->fd, msgptr, nleft)) < 0) {
			printf("error: %s\n", strerror(errno));
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
 	int err = 0;
 	int nbytes, nleft, nread;

 	nbytes = msglen;
 	nleft = nbytes;

 	while(nleft > 0) {
	 	if((nread = read(sckt->fd, msgptr, nleft)) == -1) {
			printf("error: %s\n",strerror(errno));
			err = -1;
			break;
		}
		else if(nread == 0) // closed by peer
			break;
		nleft = nleft - nread;
		msgptr = msgptr + nread; 
	}

	return err;
 }