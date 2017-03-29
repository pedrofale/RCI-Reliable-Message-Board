/******************************************************************************
 * File Name: comm_rmb.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     comm_rmb: interface communication handling methods on a RMB terminal
 *
 * DESCRIPTION
 *      Interface for methods for communication via a RMB terminal, which can be
 *		with the ID server or with a message server, both via UDP
 * 
 *****************************************************************************/

#ifndef _COMM_RMB_H_
#define _COMM_RMB_H_

#include "comm_utils.h"
#include "msgserv.h"

#define MAX_BUFF_SIZE 500
#define UDP_TIMEOUT_SECS 10

#define GET_SERVERS "GET_SERVERS"

#define PUBLISH_MESSAGE "PUBLISH "
#define GET_MESSAGES "GET_MESSAGES "
#define MESSAGES "MESSAGES\n"

int COMMRMB_get_servers(SOCKET* socket, char *str, int strlen);

int COMMRMB_publish_message(SOCKET *socket, char *msg);
int COMMRMB_get_n_messages(SOCKET *socket, int n);
int COMMRMB_read_messages(SOCKET* socket);

#endif