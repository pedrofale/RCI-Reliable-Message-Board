#ifndef _COMM_MSGSERV_H_
#define _COMM_MSGSERV_H_

#include "comm_utils.h"
#include "msgserv.h"

#define MAX_BUFF_SIZE 500
#define TIMEOUT 10

#define PUBLISH_MESSAGE "PUBLISH "
#define GET_MESSAGES "GET_MESSAGES "
#define MESSAGES "MESSAGES\n"

#define SGET_MESSAGES "SGET_MESSAGES\n"
#define SMESSAGES "SMESSAGES\n"

int COMMMSGSERV_get_server_list(SOCKET* idserv_clientsocket, char *server_list, int server_list_len);
void COMMMSGSERV_connect_msgservs(SOCKET *msgserv_clientsockets[], char *server_list);

int read_from_terminal(SOCKET *sckt, MSGSERV *msgserv, SOCKET *msgserv_clientsockets[], int num_msgservs, char *new_message);
int COMMMSGSERV_distribute_message(SOCKET *msgserv_clientsockets[], int num_msgservs, MSGSERV *msgserv);
int read_from_msgserv(SOCKET*, MSGSERV*);
int COMMMSGSERV_request_messages_for_msgserv(SOCKET**, int num_msgservs, MSGSERV*);

#endif