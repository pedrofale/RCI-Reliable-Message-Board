﻿/******************************************************************************
 * File Name: comm_msgserv.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     comm_msgserv: communication handling on a MSG server
 *
 * DESCRIPTION
 *      Implementation of methods for communication via a MSG server, which can be
 *		with terminals via UDP, with the ID server via UDP or with other MSG servers
 *		via TCP
 * 
 *****************************************************************************/

#include "comm_msgserv.h"


/********************************************************************************
 * Communication protocol between MSG servers and ID server
 *	- MSG server to ID server: REG name;ip;upt;tpt\n
 *	- MSG server to ID server: GET_SERVERS
 *	- ID server to MSG server: SERVERS\nname;ip;upt;tpt\n
 ********************************************************************************/

/*
 * Function: COMMMSGSERV_register 
 * ------------------------------------------------------------------------------
 * Send "REG name;ip;upt;tpt" to the ID server.
 *
 *  socket: UDP socket to talk with the ID server
 *	msgserv: this message server's information
 *
 *  returns:  < 0 if there was an error
 */

 int COMMMSGSERV_register(SOCKET *socket, MSGSERV *msgserv) {
 	char msg[MAX_BUFF_SIZE] = "";
 	char aux[5] = "";
  	char str[20] = "";

 	strcpy(msg, REGISTER);
 	strcpy(aux, ";");

 	strcat(msg, MSGSERV_get_name(msgserv)); strcat(msg, aux);
 	strcat(msg, MSGSERV_get_ip_str(msgserv)); strcat(msg, aux);
 	MSGSERV_get_upt_str(msgserv, str);
 	strcat(msg, str); strcat(msg, aux);
 	MSGSERV_get_tpt_str(msgserv, str);
 	strcat(msg, str);

 	if(sendmsg_udp(socket, msg, MAX_BUFF_SIZE) == -1) { 
 		fprintf(stderr, "Error registering message server in the ID server\n");
 		return -1;
 	}

 	return 0;
 }

/*
 * Function: COMMMSGSERV_get_servers 
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

int COMMMSGSERV_get_servers(SOCKET* socket, char *server_list, int server_list_len, int tries) {
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

 /*
 * Function: COMMMSGSERV_get_num_msgservs
 * ------------------------------------------------------------------------------
 * Returns the number of message servers registered in the ID server.
 *
 *	str: string containing the list of servers
 *
 *  returns: the number of registered message servers
 */

int COMMMSGSERV_get_num_msgservs(char *str) {
 	int num_msgservs, i;

	// number of MSG servers is the number of \n - 1
	num_msgservs = -1;
	for(i = 0; i < strlen(str); i++)
		if(str[i] == '\n') num_msgservs++;

 	return num_msgservs;
}


/********************************************************************************
 *	Communication protocol between MSG servers and terminals
 *	- terminal to MSG server: PUBLISH message
 *	- terminal to MSG server: GET_MESSAGES n
 *	- MSG server to terminal: MESSAGES\n(message\n)*
 ********************************************************************************/


/*
 * Function:  COMMMSGSERV_read_from_terminal 
 * ------------------------------------------------------------------------------
 * Reads what has been sent to the UDP socket this message server holds at port UPT.
 * If a message has been published by a terminal, this message is added to this
 * message server and sent to all the others it holds a TCP session with.
 * If a terminal asked for the message list, send it to them.
 *
 *  sckt: UDP socket to talk with the terminals
 *	msgserv: this message server's information
 *	msgserv_clientsockets: array of sockets to talk with other message servers
 * 	num_msgservs: number of message servers registered in the ID server (this
 * 				  doesn't decrease if a message server has disconnected)
 *
 *  returns: < 0 if there was an error
 */

int COMMMSGSERV_read_from_terminal(SOCKET *sckt, MSGSERV *msgserv, SOCKET **msgserv_clientsockets, int num_msgservs) {
	int err = 0;
	char msg[MAX_BUFF_SIZE] = "";

	readmsg_udp(sckt, msg, sizeof(msg));

	if(!strncmp(msg, PUBLISH_MESSAGE, strlen(PUBLISH_MESSAGE))) {
		if(publish_message(msg, msgserv) == -1)
			err = -1;
		if(COMMMSGSERV_distribute_message(msgserv_clientsockets, num_msgservs, msgserv) < 0)
			fprintf(stderr, "Error: error distributing new message along all message servers.\n");
	}
	else if(!strncmp(msg, GET_MESSAGES, strlen(GET_MESSAGES))) {
		if(send_messages_to_terminal(msg, sckt, msgserv) == -1)
			err = -2;
	}
	else {
		err = -3;
		fprintf(stderr, "Error: unrecognized request from a terminal.\n");
	}

	return err;
}

/*
 * Function:  publish_message 
 * ------------------------------------------------------------------------------
 * Publishes the message sent by a terminal in this message server, incrementing LC
 * and assigning it to the new message.
 *
 *  buffer: string containing the message from the terminal
 *	msgserv: msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int publish_message(char *buffer, MSGSERV *msgserv) {
	int err = 0;
	int lc = 0;
	char new_message[MAX_BUFF_SIZE] = "";

	sscanf(buffer, "%*s %[^\t\n]", new_message); // get message from buffer
	strcat(new_message, "\n");
	
	MSGSERV_increment_lc(msgserv);
	err = MSGSERV_add_message_str_lc(msgserv, new_message, MSGSERV_get_lc(msgserv));

	if(err == -1)
		fprintf(stderr, "Error: error adding message to message server.\n");

	return err;	
}

/*
 * Function:  send_messages_to_terminal 
 * ------------------------------------------------------------------------------
 * Sends the number of requested messages to the terminal who requested them.
 *
 *  buffer: string containing the message from the terminal
 *	sckt: socket to talk with a terminal via UDP
 *	msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int send_messages_to_terminal(char *buffer, SOCKET *sckt, MSGSERV *msgserv) {
	char resp[MAX_BUFF_SIZE] = "";
	int n, aux, i;
	int err = 0;

	// get the number of wanted messages
	sscanf(buffer, "%*s %d", &n);

	if(n > MSGSERV_get_num_messages(msgserv))
		n = MSGSERV_get_num_messages(msgserv);

	strcpy(resp, MESSAGES);

	for(i = n; i > 0; i--) {
		aux = MSGSERV_get_nth_latest_index(msgserv, i);
		strcat(resp, MSGSERV_get_message_str(msgserv, aux));
	}

	if(sendmsg_udp(sckt, resp, sizeof(resp)) == -1) {
		err = -1;
		fprintf(stderr, "Error: error sending message to terminal\n");
	}

	return err;
}

/********************************************************************************
 *	Communication protocol between MSG servers
 *	- MSG server to MSG server: SGET_MESSAGES\n
 *	- MSG server to MSG server: SMESSAGES\n(clock;message)*\n
 ********************************************************************************/

/*
 * Function: COMMMSGSERV_connect_msgservs
 * ------------------------------------------------------------------------------
 * Estabilish a TCP session with all the message servers described in server_list.
 *
 *  msgserv_clientsockets: array of TCP sockets, one socket for each message server
 *	server_list: string holding the list of servers in the format sent by the ID server
 *	msgserv: this message server
 *
 *  returns: void
 */

void COMMMSGSERV_connect_msgservs(SOCKET *msgserv_clientsockets[], char *server_list, MSGSERV *msgserv) {
  	char * curr_line = server_list;
  	int cnt = 0;
  	int i = 0;

  	while(curr_line) {
  		char *next_line = strchr(curr_line, '\n');
  		int curr_line_len = next_line ? (next_line-curr_line) : strlen(curr_line);
  		char *temp_str = malloc(curr_line_len + 1);
  		if(cnt > 0) {
  			i++;
	  		if(temp_str) {
	  			memcpy(temp_str, curr_line, curr_line_len);

	  			// temp_str contains name;ip;upt;tpt\0
	  			temp_str[curr_line_len] = '\0';

	  			if(strstr(temp_str, ";") != NULL) {
		  			MSGSERVID *msgserv_id = MSGSERVID_create();
		  			// read in the IP address and TCP port of the MSG server in current line
					MSGSERVID_set_ID_from_reg(msgserv_id, temp_str);
					// check if not trying to connect to self
					if(!(MSGSERVID_compare_ip(msgserv_id, MSGSERV_get_ID(msgserv)) && MSGSERVID_compare_tpt(msgserv_id, MSGSERV_get_ID(msgserv))))
		  				msgserv_clientsockets[i - 1] = create_tcp_client_socket(MSGSERVID_get_ip(msgserv_id), MSGSERVID_get_tpt(msgserv_id));
		  			else msgserv_clientsockets[i - 1] = NULL;
		  			MSGSERVID_free(msgserv_id);
	  			}
	  		}
	  		else fprintf(stderr, "Error: malloc() failed\n");
	  	}
	  	free(temp_str);
  		cnt++;
      	curr_line = next_line ? (next_line + 1) : NULL;
  	}
 }

/*
 * Function: COMMMSGSERV_read_from_msgserv
 * ------------------------------------------------------------------------------
 * Reads what has been sent to the TCP socket this message server holds at port TPT.
 * If a message server has asked for the messages in this server, send them.
 * If a message server has sent messages to this message server, add them to this one.
 *
 *  sckt: TCP server socket to listen to TCP requests
 *	msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int COMMMSGSERV_read_from_msgserv(SOCKET *sckt, MSGSERV *msgserv) {
	int err = 0;
	int ret = 0;
	char msg[MAX_BUFF_SIZE] = "";

	if((ret = readmsg_tcp(sckt, msg, MAX_BUFF_SIZE)) < 0)
		err = -1;
	else if(ret == 0) // connection closed by peer
		err = -2;
		
	if(err == 0) {
		if(!strncmp(msg, SGET_MESSAGES, strlen(SGET_MESSAGES))) {
			if(send_messages_to_msgserv(sckt, msgserv) == -1)
				err = -1;
		}
		else if(!strncmp(msg, SMESSAGES, strlen(SMESSAGES))) {
			if(add_smessages(msgserv, msg) == -1)
				err = -1;
		}
		else {
			err = 1;
			fprintf(stderr, "Unrecognized request from a message server.\n");
		}
	}

	return err;
}

/*
 * Function: send_messages_to_msgserv
 * ------------------------------------------------------------------------------
 * Sends all messages in this server to a TCP socket.
 *
 *  sckt: TCP socket to read/write TCP data
 *	msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int send_messages_to_msgserv(SOCKET *sckt, MSGSERV *msgserv) {
	char resp[MAX_BUFF_SIZE] = "";
	int n, aux, i;
	int err = 0;
	char lcstr[10];
	int lc = 0;

	// get the number of available messages
	n = MSGSERV_get_num_messages(msgserv);

	strcpy(resp, SMESSAGES);

	for(i = n; i > 0; i--) {
		aux = MSGSERV_get_nth_latest_index(msgserv, i);
		lc = MSGSERV_get_message_lc(msgserv, aux);
		sprintf(lcstr, "%d", lc);
		strcat(resp, lcstr);
		strcat(resp, ";");
		strcat(resp, MSGSERV_get_message_str(msgserv, aux));
	}

	if(writemsg_tcp(sckt, resp, sizeof(resp)) == -1) {
		err = -1;
		fprintf(stderr, "Error: error sending message list to message server.\n");
	}

	return err;
}

/*
 * Function: COMMMSGSERV_distribute_message
 * ------------------------------------------------------------------------------
 * Sends a message to all the message servers this one has a TCP session with.
 *
 *  msgserv_clientsockets: array of TCP sockets, one socket for each message server
 * 	num_msgservs: number of message servers registered in the ID server (this
 * 				  doesn't decrease if a message server has disconnected)
 *	msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int COMMMSGSERV_distribute_message(SOCKET *msgserv_clientsockets[], int num_msgservs, MSGSERV *msgserv) {
	int err = 0;
	char new_message[MAX_BUFF_SIZE] = "";
	char aux[10] = "";
	int i;

	strcpy(new_message, SMESSAGES);

	sprintf(aux, "%d", MSGSERV_get_message_lc(msgserv, MSGSERV_get_latest_message_index(msgserv)));
	strcat(new_message, aux);
	strcat(new_message, ";");
	strcat(new_message, MSGSERV_get_message_str(msgserv, MSGSERV_get_latest_message_index(msgserv)));
	
	for(i = 0; i < num_msgservs; i++) {
		if(msgserv_clientsockets[i] != NULL)
			if(writemsg_tcp(msgserv_clientsockets[i], new_message, sizeof(new_message)) == -1) {
				err = -1;
				fprintf(stderr, "Error: error sending new message to message server.\n");
			}
	}

	return err;
}

/*
 * Function: add_smessages
 * ------------------------------------------------------------------------------
 * Adds all the messages in a MESSAGE\n(message\n) string to this message server.
 *
 *	msgserv: this message server's information
 *	string: string containing messages
 *
 *  returns: < 0 if there was an error
 */

int add_smessages(MSGSERV *msgserv, char *string) {
	int err = 0;
	int cnt = 0;
	char message[MAX_BUFF_SIZE] = "";
	int msglc = 0;

	char *curr_line = string;

  	while(curr_line) {
  		char *next_line = strchr(curr_line, '\n');
  		int curr_line_len = next_line ? (next_line-curr_line) : strlen(curr_line);
  		char *temp_str = malloc(curr_line_len + 1);
  		if(cnt > 0)
	  		if(temp_str) {
	  			memcpy(temp_str, curr_line, curr_line_len);

	  			// temp_str contains lc;message\0
	  			temp_str[curr_line_len] = '\0';

	  			if(strstr(temp_str, ";")) {
		  			sscanf(temp_str, "%d;%[^\t]", &msglc, message);
		  			strcat(message, "\n");
		  			MSGSERV_add_message_str_lc(msgserv, message, msglc);

		  			int new_lc = msglc > MSGSERV_get_lc(msgserv) ? msglc : MSGSERV_get_lc(msgserv);
		  			MSGSERV_set_lc(msgserv, new_lc + 1);
		  		}
	  		}
	  		else {
	  			fprintf(stderr, "Error: malloc() failed\n");
	  			err = -1;
	  		}

	  	free(temp_str);
      	curr_line = next_line ? (next_line + 1) : NULL;
      	cnt++;
  	}
  	return err;
}

/*
 * Function: COMMMSGSERV_request_messages_for_msgserv
 * ------------------------------------------------------------------------------
 * Sends a sSGET_MESSAGES request to all message servers this one has a TCP session with
 *
 *	sckt: array of TCP sockets
 * 	num_msgservs: number of message servers registered in the ID server (this
 * 				  doesn't decrease if a message server has disconnected)
  *	msgserv: this message server's information
 *
 *  returns: < 0 if there was an error
 */

int COMMMSGSERV_request_messages_for_msgserv(SOCKET *sckt[], int num_msgservs, MSGSERV *msgserv) {
	char resp[MAX_BUFF_SIZE] = "";
	char msg[MAX_BUFF_SIZE] = "";
	int err = 0;
	int i;

	struct timeval tv;
	tv.tv_sec = 30;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	strcpy(msg, SGET_MESSAGES);

	for(i = 0; i < num_msgservs; i++) {
		if(sckt[i] != NULL) {	
			setsockopt(sckt[i], SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));		
			if(writemsg_tcp(sckt[i], msg, MAX_BUFF_SIZE) == -1) {
				err = -1;
				fprintf(stderr, "Error: error requesting message server for messages.\n");
			}
			else if(readmsg_tcp(sckt[i], resp, MAX_BUFF_SIZE) == -1) {
				err = -2;
				fprintf(stderr, "Error: error receiving message list.\n");
			}
			if(strstr(resp, MESSAGES) == NULL) {
				SOCKET_close(sckt[i]);
				sckt[i] = NULL;
			}
		}
	}

	if(strstr(resp, MESSAGES)) {
		printf("Message list:\n%s\n", resp);
		
		// Parse messages from response string and store them in this MSG server
		if(add_smessages(msgserv, resp) == -1) {
			err = -3;
			fprintf(stderr, "Error: error adding messages.\n");
		}
	}

	return err;
}
