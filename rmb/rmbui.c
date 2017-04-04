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
 int RMBUI_show_servers(SOCKET *socket) {
 	int err = 0;
 	char str[200] = "";
 	
 	err = COMMRMB_get_servers(socket, str, 200);

 	printf("%s\n", str);

 	return err;
 }

 int RMBUI_publish(SOCKET *socket, char *user_input) {
 	char msg[128];
 	char pub[128]="PUBLISH ";

 	sscanf(user_input, "%*s %[^\t\n]", msg);

 	return COMMRMB_publish_message(socket, msg);
 }


 int RMBUI_show_n_messages(SOCKET *socket, char *user_input) {
 	int n_str[10];

 	sscanf(user_input, "%*s %[^\t\n]", n_str);
 	return COMMRMB_get_n_messages(socket, atoi(n_str));
 }
