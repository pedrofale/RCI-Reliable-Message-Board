/******************************************************************************
 * File Name: msgservui.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservui - user intefarce commands
 *
 * DESCRIPTION
 *      Interface of the functions corresponding to commands available to the 
 *		msgserv user
 *
 * METHODS
 *		join: register the server in the ID server
 *		show_servers: list the identities of all the other MSG servers this one
 *					  has a TCP session estabilished with
 *		show_messages: list all the messages this server has stored, ordered by
 *					   their LCs
 *		exit: exit the app
 *
 *****************************************************************************/

#include <string.h>
#include "msgserv.h"
#include "comm_utils.h"

#define MAX_MSG_LEN 128

 /* send "REG name;ip;upt;tpt" to siip at port sipt via UDP */
 int join(MSGSERV*, SOCKET*);

 /* send "GET_SERVERS" to siip at port sipt via UDP */
 //int show_servers(MSGSERV*, char*);

 int show_messages();

 int exitapp();