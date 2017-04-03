/******************************************************************************
 * File Name: comm_rmb.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     comm_rmb: communication handling on a RMB terminal
 *
 * DESCRIPTION
 *      Implementation of methods for communication via a RMB terminal, which can be
 *		with the ID server or with a message server, both via UDP 
 * 
 *****************************************************************************/

#include "comm_rmb.h"


/********************************************************************************
 * Communication protocol between the RMB terminal and the ID server
 *	- RMB terminal to ID server: GET_SERVERS
 *	- ID server to RMB terminal: SERVERS\nname;ip;upt;tpt\n
 ********************************************************************************/


/*
 * Function: COMMRMB_get_servers 
 * ------------------------------------------------------------------------------
 * Send a UDP request to get the servers registered in the ID server.
 *
 *  socket: UDP socket to talk with the ID server
 *	server_list: string to store the list of servers
 *	server_list_len: length of server_list string
 *
 *  returns: < 0 if there was a communication error
 */

int COMMRMB_get_servers(SOCKET* socket, char *server_list, int server_list_len) {
	char msg[MAX_BUFF_SIZE] = "";
 	int err = 0;
 	int counter, i;
	int socket_fd;

 	fd_set rfds;

 	struct timeval timeout;
 	timeout.tv_sec = UDP_TIMEOUT_SECS;
 	timeout.tv_usec = 0;

 	socket_fd = SOCKET_get_fd(socket);

 	strcpy(msg, GET_SERVERS);

 	while(strstr(server_list, "SERVERS") == NULL) {
 		FD_ZERO(&rfds);
		FD_SET(socket_fd, &rfds);
		
		if(sendmsg_udp(socket, msg, sizeof(msg)) == -1) err = -1; 
		
		if((counter = select(socket_fd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &timeout)) < 0) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			err = -1;
			break;
		} else if(counter == 0) { // timeout activated, didn't get answer back from the ID server
			timeout.tv_sec = UDP_TIMEOUT_SECS;
 			timeout.tv_usec = 0;
			continue;
		}

		if(FD_ISSET(socket_fd, &rfds)) {
			if(readmsg_udp(socket, server_list, server_list_len) == -1) err = -2;
		}
 	}

 	return err;
}

/********************************************************************************
 *	Communication protocol between terminals and MSG servers
 *	- terminal to MSG server: PUBLISH message
 *	- terminal to MSG server: GET_MESSAGES n
 *	- MSG server to terminal: MESSAGES\n(message\n)*
 ********************************************************************************/

/*
 * Function: COMMRMB_publish_message 
 * ------------------------------------------------------------------------------
 * Send a message via UDP to a message server
 *
 *  socket: UDP socket to talk with the MSG server
 *	msg: message to publish in the MSG server
 *
 *  returns: < 0 if there was a communication error
 */

 int COMMRMB_publish_message(SOCKET *socket, char *msg){
 	char pub[128]="PUBLISH ";

 	strcat(pub, msg);

 	if(sendmsg_udp(socket, pub, sizeof(pub))==-1)
 		return -1;

 	return 0;
 }

/*
 * Function: COMMRMB_get_n_messages 
 * ------------------------------------------------------------------------------
 * Send a UDP request to get the a number of messages from a message server
 *
 *  socket: UDP socket to talk with the MSG server
 *	n: number of messages to retrieve
 *
 *  returns: < 0 if there was a communication error
 */

int COMMRMB_get_n_messages(SOCKET *socket, int n) {
	int err = 0;
	char show_n[MAX_BUFF_SIZE]="GET_MESSAGES ";
	char n_str[10] = "";

	sprintf(n_str, "%d", n);
	strcat(show_n, n_str);

	if(sendmsg_udp(socket, show_n, sizeof(show_n)) == -1)
		err = -1;

	return err;
}

/*
 * Function: COMMRMB_read_messages 
 * ------------------------------------------------------------------------------
 * Read from the MSG server socket the messages it sent and print them.
 *
 *  socket: UDP socket to talk with the MSG server
 *
 *  returns: < 0 if there was a communication error
 */

int COMMRMB_read_messages(SOCKET* socket) {
	char ret_msg[MAX_BUFF_SIZE];
	int err = 0;

	if(readmsg_udp(socket, ret_msg, sizeof(ret_msg)) == -1) 
		err = -1;

 	printf("%s\n", ret_msg);
 	
 	return err;

 }
