/******************************************************************************
 * File Name: msgservui.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservui - user interface commands
 *
 * DESCRIPTION
 *      Interface of the functions corresponding to commands available to the 
 *		msgserv user
 *
 * METHODS
 *		MSGSERVUI_join: register the server in the ID server
 *		MSGSERVUI_show_servers: list the identities of all the other MSG servers this one
 *					  has a TCP session estabilished with
 *		MSGSERVUI_show_messages: list all the messages this server has stored, ordered by
 *					   their LCs
 *****************************************************************************/
#ifndef _MSGSERVUI_H_
#define _MSGSERVUI_H_

#include <stdio.h>
#include <string.h>
#include "msgserv.h"
#include "comm_utils.h"
#include "comm_msgserv.h"

#define MAX_MSG_LEN 600

 /* send "REG name;ip;upt;tpt" to siip at port sipt via UDP */
 int MSGSERVUI_join(MSGSERV*, SOCKET*);

 /* send "GET_SERVERS" to siip at port sipt via UDP */
 int MSGSERVUI_show_servers(SOCKET*);

 int MSGSERVUI_show_messages();

 #endif