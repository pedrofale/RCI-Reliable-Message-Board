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
#define UDP_TIMEOUT_SECS 1

#define REGISTER "REG "
#define GET_SERVERS "GET_SERVERS"
#define SERVERS "SERVERS\n"

#define PUBLISH_MESSAGE "PUBLISH "
#define GET_MESSAGES "GET_MESSAGES "
#define MESSAGES "MESSAGES\n"

#define SGET_MESSAGES "SGET_MESSAGES\n"
#define SMESSAGES "SMESSAGES\n"

int COMMMSGSERV_register(SOCKET *socket, MSGSERV *msgserv);
int COMMMSGSERV_get_servers(SOCKET* socket, char *server_list, int server_list_len, int tries);
int COMMMSGSERV_get_num_msgservs(char *str);

void COMMMSGSERV_connect_msgservs(SOCKET *msgserv_clientsockets[], char *server_list, MSGSERV *msgserv);

int COMMMSGSERV_read_from_terminal(SOCKET *sckt, MSGSERV *msgserv, SOCKET *msgserv_clientsockets[], int num_msgservs);
int COMMMSGSERV_distribute_message(SOCKET *msgserv_clientsockets[], int num_msgservs, MSGSERV *msgserv);
int COMMMSGSERV_read_from_msgserv(SOCKET*, MSGSERV*);
int COMMMSGSERV_request_messages_for_msgserv(SOCKET**, int num_msgservs, MSGSERV*);

#endif