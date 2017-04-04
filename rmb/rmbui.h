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

#ifndef _RMBUI_H_
#define _RMBUI_H_

#include "rmb.h"
#include "comm_utils.h"
#include "msgservid.h"
//#include "string.h"


#define MAX_BUFF_SIZE 500
#define UDP_TIMEOUT_SECS 10

int RMBUI_show_servers(SOCKET* socket);
int RMBUI_publish(SOCKET *socket, char *user_input);
int RMBUI_show_n_messages(SOCKET *socket, char *user_input);

#endif