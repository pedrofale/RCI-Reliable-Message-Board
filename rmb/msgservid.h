/******************************************************************************
 * File Name: msgservid.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
int MSGSERVID_get_upt_str(MSGSERVID*, char*);

void MSGSERVID_set_tpt(MSGSERVID*, int);
int MSGSERVID_get_tpt(MSGSERVID*);
int MSGSERVID_get_tpt_str(MSGSERVID*, char*);

int MSGSERVID_set_ID_from_reg(MSGSERVID *p, char *reg_str);

int MSGSERVID_compare_ID(MSGSERVID*, MSGSERVID*);
int MSGSERVID_compare_ip(MSGSERVID*, MSGSERVID*);
int MSGSERVID_compare_upt(MSGSERVID*, MSGSERVID*);
int MSGSERVID_compare_tpt(MSGSERVID*, MSGSERVID*);

#endif