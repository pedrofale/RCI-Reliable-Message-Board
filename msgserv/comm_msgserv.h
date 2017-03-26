/******************************************************************************
 * File Name: comm_msgserv.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     comm_msgserv: interface communication handling methods on a MSG server
 *
 * DESCRIPTION
 *      Interface for methods for communication via a MSG server, which can be
 *		with terminals via UDP, with the ID server via UDP or with other MSG servers
 *		via TCP
 * 
 *****************************************************************************/

#ifndef _COMM_MSGSERV_H_
#define _COMM_MSGSERV_H_

#include "comm_utils.h"
#include "msgserv.h"

#define MAX_BUFF_SIZE 500

#define GET_SERVERS "GET_SERVERS"

#define PUBLISH_MESSAGE "PUBLISH "
#define GET_MESSAGES "GET_MESSAGES "
#define MESSAGES "MESSAGES\n"

#define SGET_MESSAGES "SGET_MESSAGES\n"
#define SMESSAGES "SMESSAGES\n"

int COMMMSGSERV_get_server_list(SOCKET* idserv_clientsocket, char *server_list, int server_list_len);
int COMMMSGSERV_register(SOCKET *socket, MSGSERV *msgserv);
int COMMMSGSERV_show_servers(MSGSERV *p, SOCKET* socket, char *str, int strlen);

void COMMMSGSERV_connect_msgservs(SOCKET *msgserv_clientsockets[], char *server_list);

int COMMMSGSERV_read_from_terminal(SOCKET *sckt, MSGSERV *msgserv, SOCKET *msgserv_clientsockets[], int num_msgservs);
int COMMMSGSERV_distribute_message(SOCKET *msgserv_clientsockets[], int num_msgservs, MSGSERV *msgserv);
int COMMMSGSERV_read_from_msgserv(SOCKET*, MSGSERV*);
int COMMMSGSERV_request_messages_for_msgserv(SOCKET**, int num_msgservs, MSGSERV*);

#endif