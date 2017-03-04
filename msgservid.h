/******************************************************************************
 * File Name: msgservid.h
 * Author: Pedro Ferreira, João Nobre, Filipe Ferreira IST MEEC
 *
 * NAME
 *     msgservid - interface for the identification structure of a MSG server
 *
 * DESCRIPTION
 *      Interface for creating and interacting with the identification of an 
 *		MSG server
 *
 * STRUCTURE FIELDS 
 *		name: (char*) name of the MSG server
 *		ip: (struct in_addr*) IPv4 address of the MSG server
 *		upt: (int) UDP port of the MSG server to listen to terminal requests
 *		tpt: (int) TCP port of the MSG server to listen to requests from other
 *			 MSG servers
 *
 *****************************************************************************/

#ifndef _MSGSERVID_H_
#define _MSGSERVID_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_NAME_LEN 20	

typedef struct sMSGSERVID MSGSERVID;

MSGSERVID* MSGSERVID_create();
void MSGSERVID_free(MSGSERVID*);

int MSGSERVID_set_name(MSGSERVID*, char*);
char* MSGSERVID_get_name(MSGSERVID*);

void MSGSERVID_set_ip(MSGSERVID*, struct in_addr);
int MSGSERVID_set_ip_str(MSGSERVID*, char*);
struct in_addr MSGSERVID_get_ip(MSGSERVID*);
char* MSGSERVID_get_ip_str(MSGSERVID*);

void MSGSERVID_set_upt(MSGSERVID*, int);
int MSGSERVID_get_upt(MSGSERVID*);

void MSGSERVID_set_tpt(MSGSERVID*, int);
int MSGSERVID_get_tpt(MSGSERVID*);

#endif