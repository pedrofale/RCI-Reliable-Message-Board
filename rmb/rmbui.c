/******************************************************************************
 * File Name: rmbui.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     rmbui - user interface commands
 *
 * DESCRIPTION
 *      Implementation of the functions corresponding to commands available to the 
 *		rmb user
 *
 * METHODS
 *		RMBUI_show_servers: list the identities of all the MSG servers registered
 *								in the ID server
 *		RMBUI_publish: publish a message in a MSG server
 *		RMBUI_show_n_messages: ask for the latest n messages in a MSG server
 *****************************************************************************/

#include "rmbui.h"

/* send "GET_SERVERS" to siip at port sipt via UDP */
 int RMBUI_show_servers(SOCKET *socket, int tries) {
 	int err = 0;
 	char str[500] = "";
 	
 	if((err = COMMRMB_get_servers(socket, str, 500, tries))> 0)
		printf("%s\n", str);
	else {
		fprintf(stderr, "Couldn't reach ID server.\n");
	}

 	return err;
 }

/*
 * Function: RMBUI_publish 
 * ------------------------------------------------------------------------------
 * Publish a message in the message server. Check if it is online by sending a 
 * GET_MESSAGES 1. If no answer is received, the server is offline. If an answer
 * is received but the message wasn't published (not contained in the MESSAGES 
 * response), try again.
 *
 *  socket: UDP socket to talk with the MSG server
 *	user_input: string containing the "publish (message)" to publish in the MSG server
 *	tries: number of times to try to publish
 *
 *  returns: 1 if the MSG server is not online anymore
 *			 -1 if there was an error
 *			 2 if there is no connection to a MSG server
 *			 3 if the server is alive but didn't get the message
 *			
 */
 int RMBUI_publish(SOCKET *socket, char *user_input, int tries) {
 	char msg[500] = "";
 	char pub[128]=PUBLISH_MESSAGE;
 	char messages[500]="";
 	char received[500]="";
 	int maxfd = 0, fd;
 	int counter = 0;
 	int err = 0;
 	fd_set rfds;
 	struct timeval timeout;
 	
 	// to check if the message server is still alive
 	timeout.tv_sec = UDP_TIMEOUT_SECS;
 	timeout.tv_usec = 0;

 	// check if there is a connection to a server
 	if(socket != NULL) {

	 	fd = SOCKET_get_fd(socket);
	 	maxfd = fd;

	 	sscanf(user_input, "%*s %[^\t\n]", msg);

	 	while(tries > 0 && err >= 0) {
	 		tries--;

		 	err = COMMRMB_publish_message(socket, msg);
		 	COMMRMB_get_n_messages(socket, 1); // get latest message from message server

			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
			if((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &timeout)) < 0) {
				fprintf(stderr, "error: %s\n", strerror(errno));
				err = -1;
				break;
			} else if(counter == 0) { // timeout activated: the server is not online
				timeout.tv_sec = UDP_TIMEOUT_SECS;
	 			timeout.tv_usec = 0;
	 			err = 1;
				continue;
			}

			if(FD_ISSET(fd, &rfds)) {
				COMMRMB_read_messages(socket, messages, 500);
				// if sent message was successfully stored in the message server
				if(strstr(messages, msg) != NULL)
					break;
				else // the server didn't get the message
					err = 3;
				strcpy(messages, "");
			}
	    }
	 } else err = 2; // there is no connection to a server

	 return err;
}

/*
 * Function: RMBUI_show_n_messages 
 * ------------------------------------------------------------------------------
 * Ask the MSG server for the last n messages. Check if it is online by waiting for
 * the response for tries*UDP_TIMEOUT_SECS.
 *
 *  socket: UDP socket to talk with the MSG server
 *	user_input: string containing the "publish (message)" to publish in the MSG server
 *	tries: number of times to try to publish
 *
 *  returns: 1 if the MSG server is not online anymore
 *			 -1 if there was an error
 *			 2 if there is no connection to a MSG server
 *			
 */
 int RMBUI_show_n_messages(SOCKET *socket, char *user_input, int tries) {
 	int n_str[10];
 	int maxfd = 0, fd;
 	int counter = 0;
 	int err = 0;
 	char messages[500]="";
 	fd_set rfds;
 	struct timeval timeout;
 	
 	// to check if the message server is still alive
 	timeout.tv_sec = UDP_TIMEOUT_SECS;
 	timeout.tv_usec = 0;

 	// check if there is a connection to a server
 	if(socket != NULL) {

	 	fd = SOCKET_get_fd(socket);
	 	maxfd = fd;

	 	// check how many messages the user requested
	 	sscanf(user_input, "%*s %[^\t\n]", n_str);

	 	while(tries > 0 && err >= 0) {
	 		tries--;

		 	COMMRMB_get_n_messages(socket, atoi(n_str));

			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
			if((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &timeout)) < 0) {
				fprintf(stderr, "error: %s\n", strerror(errno));
				err = -1;
				break;
			} else if(counter == 0) { // timeout activated: the server is not online
				timeout.tv_sec = UDP_TIMEOUT_SECS;
	 			timeout.tv_usec = 0;
	 			err = 1;
				continue;
			}

			if(FD_ISSET(fd, &rfds)) {
				// successfully got the messages
				err = COMMRMB_read_messages(socket, messages, 500);
				if(err < 0) fprintf(stderr, "Error getting messages from server.\n");
				else printf("%s", messages);
				break;
			}
	    }
	 } else err = 2; // there is no connection to a server

 	return err;
 }

