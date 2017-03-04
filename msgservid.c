/******************************************************************************
 * File Name: msgservid.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgservid - interface implementation for the identification structure 
 *					of a MSG server
 *
 * DESCRIPTION
 *      Methods for creating and interacting with the identification of an 
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

#include "msgservid.h"
#include <string.h>
#include <stdlib.h>

struct sMSGSERVID {
	char name[MAX_NAME_LEN];
	struct in_addr ip;
	int upt; 
	int tpt;
};

MSGSERVID* MSGSERVID_create() {
	return malloc(sizeof(MSGSERVID)); 
}

void MSGSERVID_free(MSGSERVID* p) {
	free(p);
}

int MSGSERVID_set_name(MSGSERVID* p, char* name) {
	if(strcpy(p->name, name) == NULL)
		return -1;

	return 0;
}

char* MSGSERVID_get_name(MSGSERVID* p) {
	return p->name;
}

void MSGSERVID_set_ip(MSGSERVID* p, struct in_addr ip) {
	p->ip = ip;
}

int MSGSERVID_set_ip_str(MSGSERVID* p, char* ip_str) {
	struct in_addr *a = malloc(sizeof(struct in_addr));
	if (inet_aton(ip_str, a) == 0) { free(a); return -1; }
	MSGSERVID_set_ip(p, *a);
	free(a);
	return 0;
}

struct in_addr MSGSERVID_get_ip(MSGSERVID* p) {
	return p->ip;
}

char* MSGSERVID_get_ip_str(MSGSERVID* p) {
	return inet_ntoa(p->ip);
}

void MSGSERVID_set_upt(MSGSERVID* p, int upt) {
	p->upt = upt;
}

int MSGSERVID_get_upt(MSGSERVID* p) {
	return p->upt;
}

void MSGSERVID_set_tpt(MSGSERVID* p, int tpt) { 
	p->tpt = tpt;
}

int MSGSERVID_get_tpt(MSGSERVID* p) {
	return p->tpt;
}