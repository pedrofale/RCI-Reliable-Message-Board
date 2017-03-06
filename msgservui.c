/******************************************************************************
 * File Name: msgservui.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservui - user intefarce commands
 *
 * DESCRIPTION
 *      Implementation of the commands available to the msgserv user
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

#include "msgservui.h"
#include <stdio.h>
#include <string.h>

/* send "REG name;ip;upt;tpt" to siip at port sipt via UDP */
 int join(MSGSERV *p, SOCKET *socket) {
 	char msg[MAX_MSG_LEN];
 	char aux[5];
  	char str[20];
  	char resp[MAX_MSG_LEN];

 	strcpy(msg, "REG ");
 	strcpy(aux, ";");

 	strcat(msg, MSGSERV_get_name(p)); strcat(msg, aux);
 	strcat(msg, MSGSERV_get_ip_str(p)); strcat(msg, aux);
 	MSGSERV_get_upt_str(p, str);
 	strcat(msg, str); strcat(msg, aux);
 	MSGSERV_get_tpt_str(p, str);
 	strcat(msg, str);

 	if(sendmsg_udp(socket, msg, sizeof(msg)) == -1) return -1;
 	printf(">> Sent: %s\n", msg);

 	//if(readmsg_udp(socket, resp, sizeof(msg)) == -1) return -1;
 	//printf(">> Received: %s\n", resp);

 	return 0;
 }

 /* send "GET_SERVERS" to siip at port sipt via UDP */
 int show_servers(MSGSERV *p, SOCKET* socket) {
 	char msg[MAX_MSG_LEN];
 	char resp[MAX_MSG_LEN];
 	int err = 0;
 	strcpy(msg, "GET_SERVERS");

 	if(sendmsg_udp(socket, msg, sizeof(msg)) == -1) err = -1;
 	printf(">> Sent: %s\n", msg);

 	if(readmsg_udp(socket, resp, sizeof(msg)) == -1) err = -2;
 	printf(">> Received: %s\n", resp);

 	/* parse message from ID server to create a list of MSG servers */

 	return err;
 }


 int show_messages() {
 	return 0;
 }

 int exitapp() {
 	return 0;
 }