/******************************************************************************
 * File Name: msgserv.h
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservid - interface for the structure of a MSG server
 *
 * DESCRIPTION
 *      Interface for creating and interacting with the an MSG server struct
 *
 * STRUCTURE FIELDS 
 *		msgserv_id: (MSGSERVERID *) identification structure of the MSG server
 *		siip: (struct in_addr) IPv4 address of the ID server to communicate with  
 *		sipt: (int) port of the ID server to communicate with via UDP
 *		m: (int) maximum number of messages stored in the MSG server
 *		r: (int) time interval between registers in the ID server in seconds
 *
 *****************************************************************************/

#ifndef _MSGSERV_H_
#define _MSGSERV_H_

#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "msgservid.h"

typedef struct sMSGSERV MSGSERV;

MSGSERV* MSGSERV_create();
void MSGSERV_free(MSGSERV*);

void MSGSERV_set_ID(MSGSERV*, MSGSERVID*);
MSGSERVID* MSGSERV_get_ID(MSGSERV*);

void MSGSERV_set_siip(MSGSERV*, struct in_addr);
int MSGSERV_set_siip_str(MSGSERV*, char*);
int MSGSERV_set_siip_dns(MSGSERV*, char*);
struct in_addr MSGSERV_get_siip(MSGSERV*);
char* MSGSERV_get_siip_str(MSGSERV *);
char* MSGSERV_get_siip_dns(MSGSERV*);

void MSGSERV_set_sipt(MSGSERV*, int);
int MSGSERV_get_sipt(MSGSERV*);

void MSGSERV_set_m(MSGSERV*, int);
int MSGSERV_get_m(MSGSERV*);

void MSGSERV_set_r(MSGSERV*, int);
int MSGSERV_get_r(MSGSERV*);

int MSGSERV_set_name(MSGSERV*, char*);
char* MSGSERV_get_name(MSGSERV*);

void MSGSERV_set_ip(MSGSERV*, struct in_addr);
int MSGSERV_set_ip_str(MSGSERV*, char*);
struct in_addr MSGSERV_get_ip(MSGSERV*);
char* MSGSERV_get_ip_str(MSGSERV*);

void MSGSERV_set_upt(MSGSERV*, int);
int MSGSERV_get_upt(MSGSERV*);
int MSGSERV_get_upt_str(MSGSERV*, char*);

void MSGSERV_set_tpt(MSGSERV*, int);
int MSGSERV_get_tpt(MSGSERV*);
int MSGSERV_get_tpt_str(MSGSERV*, char*);

#endif