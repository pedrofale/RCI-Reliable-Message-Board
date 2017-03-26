/******************************************************************************
 * File Name: msgservui.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservui - user interface commands
 *
 * DESCRIPTION
 *      Implementation of the commands available to the msgserv user
 *
 * METHODS
 *		MSGSERVUI_join: register the server in the ID server
 *		MSGSERVUI_show_servers: list the identities of all the other MSG servers this one
 *					  has a TCP session estabilished with
 *		MSGSERVUI_show_messages: list all the messages this server has stored, ordered by
 *					   their LCs
 *
 *****************************************************************************/

#include "msgservui.h"

/* send "REG name;ip;upt;tpt" to siip at port sipt via UDP */
 int MSGSERVUI_join(MSGSERV *p, SOCKET *socket) {
 	return COMMMSGSERV_register(socket, p);
 }

 /* send "GET_SERVERS" to siip at port sipt via UDP */
 int MSGSERVUI_show_servers(MSGSERV *p, SOCKET* socket) {
 	int err = 0;
 	char resp[MAX_MSG_LEN] = "";
 	
 	err = COMMMSGSERV_show_servers(p, socket, resp, MAX_MSG_LEN);

 	printf("%s\n", resp);

 	return err;
 }


 int MSGSERVUI_show_messages(MSGSERV *p) {
 	char msg[MAX_MSG_LEN] = "";
 	int err = 0;
 	int aux, n, lc;
 	char lcstr[10];

 	n = MSGSERV_get_num_messages(p);

	strcpy(msg, MESSAGES);

	for(int i = n; i > 0; i--) {
		aux = MSGSERV_get_nth_latest_index(p, i);
		lc = MSGSERV_get_message_lc(p, aux);
		sprintf(lcstr, "%d", lc);
		strcat(msg, lcstr);
		strcat(msg, ";");
		strcat(msg, MSGSERV_get_message_str(p, aux));
	}

	printf("%s\n", msg);

 	return err;
 }