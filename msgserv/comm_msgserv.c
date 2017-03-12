/******************************************************************************
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
 * METHODS
 
 *****************************************************************************/

#include "comm_msgserv.h"

 /*
 *	Communication protocol between MSG servers and terminals
 *	- terminal to MSG server: PUBLISH message
 *	- terminal to MSG server: GET_MESSAGES n
 *	- MSG server to terminal: MESSAGES\n(message\n)*
 */

int read_from_terminal(SOCKET *sckt, MSGSERV *msgserv) {
	int err = 0;
	char msg[MAX_BUFF_SIZE];

	readmsg_udp(sckt, msg, sizeof(msg));

	if(!strncmp(msg, PUBLISH_MESSAGE, sizeof(PUBLISH_MESSAGE))) {
		if(publish_message(msg, msgserv) == -1)
			err = -1;
	}
	else if(!strncmp(msg, GET_MESSAGES, sizeof(GET_MESSAGES))) {
		if(get_messages(msg, sckt, msgserv) == -1)
			err = -2;
	}
	else 
		err = -3;

	return err;
}

int publish_message(char *buffer, MSGSERV *msgserv) {
	char msg[MAX_MSG_LEN];
	int err = 0;
	int lc = 0;

	sscanf(buffer, "%*s %s", msg); // get message from buffer

	MSGSERV_increment_lc(msgserv);
	err = MSGSERV_add_message_str_lc(msgserv, msg, MSGSERV_get_lc(msgserv));

	return err;	
}

int get_messages(char *buffer, SOCKET *sckt, MSGSERV *msgserv) {
	char resp[MAX_BUFF_SIZE];
	int n, aux;
	int err = 0;

	// get the number of wanted messages
	sscanf(buffer, "%*s %d", &n);

	strcpy(resp, "MESSAGES\n");

	for(int i = n; i > 0; i++) {
		aux = MSGSERV_get_nth_latest_index(msgserv, i);
		strcat(resp, MSGSERV_get_message_str(msgserv, aux));
	}
	strcat(resp, "*");

	if(sendmsg_udp(sckt, resp, sizeof(resp)) == -1)
		err = -1;

	return err;
}

 /*
 *	Communication protocol between MSG servers
 *	- MSG server to MSG server: SGET_MESSAGES\n
 *	- MSG server to MSG server: SMESSAGES\n(clock;message)*\n
 */
