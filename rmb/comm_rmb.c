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
 *	tries: number of times the client tries to get the server list via UDP
 *
 *  returns: -1 if there was an error or if the list wasn't received
 *			  1 if the list of servers was successfully fetched
 */

int COMMRMB_get_servers(SOCKET* socket, char *server_list, int server_list_len, int tries) {
	char msg[MAX_BUFF_SIZE] = "";
 	int err = 0;
 	int counter, i;
	int socket_fd;

 	fd_set rfds;

 	struct timeval timeout;
 	timeout.tv_sec = UDP_TIMEOUT_SECS;
 	timeout.tv_usec = 0;

 	memset(server_list, 0, server_list_len);
 	socket_fd = SOCKET_get_fd(socket);

 	strcpy(msg, GET_SERVERS);

 	while(err >= 0 && tries > 0) {
 		tries--;

 		// ask for the server list
 		if(sendmsg_udp(socket, msg, MAX_BUFF_SIZE) == -1) err = -1; 

 		FD_ZERO(&rfds);
		FD_SET(socket_fd, &rfds);
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
			if(strstr(server_list, SERVERS) != NULL) {
				// received the list of servers
				err = 1;
				break;
			}
		}
 	}
 	if(tries == 0 && err != 1) // didn't receive the server list
 		err = -1;

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
 	char pub[MAX_BUFF_SIZE]="PUBLISH ";

 	strcat(pub, msg);

 	if(sendmsg_udp(socket, pub, MAX_BUFF_SIZE)==-1)
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
	char show_n[MAX_BUFF_SIZE]=GET_MESSAGES;
	char n_str[10] = "";

	sprintf(n_str, "%d", n);
	strcat(show_n, n_str);

	if(sendmsg_udp(socket, show_n, MAX_BUFF_SIZE) == -1)
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

int COMMRMB_read_messages(SOCKET* socket, char *messages, int messages_len) {
	int err = 0;

	if(readmsg_udp(socket, messages, messages_len) == -1) 
		err = -1;
 	
 	return err;

 }

 void set_msgserv_id_str(MSGSERVID *msgservid, char *server_list, int num_msgservs){
 	char * curr_line = server_list;
  	int cnt = 0;
  	int i = 0;
  	int random_index = 0;

  	random_index = rand() % num_msgservs + 1;
  	
  	while(curr_line) {
  		char *next_line = strchr(curr_line, '\n');
  		int curr_line_len = next_line ? (next_line-curr_line) : strlen(curr_line);
  		char *temp_str = malloc(curr_line_len + 1);
  		if(cnt > 0) {
	  		if(temp_str) {
	  			memcpy(temp_str, curr_line, curr_line_len);

	  			// temp_str contains name;ip;upt;tpt\0
	  			temp_str[curr_line_len] = '\0';

	  			if(strstr(temp_str, ";") != NULL) {
	  				i++;
	  				if(i == random_index)
						MSGSERVID_set_ID_from_reg(msgservid, temp_str);
	  			}
	  		}
	  		else fprintf(stderr, "Error: malloc() failed\n");
	  	}
	  	free(temp_str);
  		cnt++;
      	curr_line = next_line ? (next_line + 1) : NULL;
  	}
 }

SOCKET* COMMRMB_connect_to_message_server(SOCKET *idserv_socket, char *server_list_str) {
	SOCKET *msgserv_socket;
	int num_msgservs = 0;

	if((num_msgservs = COMMRMB_get_num_msgservs(server_list_str)) < 0) {
		fprintf(stderr, "Failed to get server list from ID server.\n");
	} else if(num_msgservs > 0) {
		MSGSERVID *msgserv_id = MSGSERVID_create();
		set_msgserv_id_str(msgserv_id, server_list_str, num_msgservs);
		msgserv_socket = create_udp_client_socket(MSGSERVID_get_ip(msgserv_id), MSGSERVID_get_upt(msgserv_id));
		MSGSERVID_free(msgserv_id);
	} else {
		msgserv_socket = NULL;
	}

	return msgserv_socket;
}

 /*
 * Function: COMMRMB_get_num_msgservs
 * ------------------------------------------------------------------------------
 * Returns the number of message servers registered in the ID server.
 *
 *	str: string containing the list of servers
 *
 *  returns: the number of registered message servers
 */

int COMMRMB_get_num_msgservs(char *str) {
 	int num_msgservs, i;

 	printf("%s", str);

	// number of MSG servers is the number of \n - 1
	num_msgservs = -2;
	for(i = 0; i < strlen(str); i++)
		if(str[i] == '\n') num_msgservs++;

	printf("%d\n", num_msgservs);

 	return num_msgservs;
}