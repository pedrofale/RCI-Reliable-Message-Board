/******************************************************************************
 * File Name: msgserv.c
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

#include "msgserv.h"

struct sMSGSERV {
	MSGSERVID *msgserv_id;
	struct in_addr siip;
	int sipt;
	int m;
	int r;
};

MSGSERV* MSGSERV_create() {
	MSGSERV *ptr = malloc(sizeof(MSGSERV));
	ptr->msgserv_id = MSGSERVID_create();
	return ptr;
}

void MSGSERV_free(MSGSERV *p) {
	MSGSERVID_free(p->msgserv_id);
	free(p);
}

void MSGSERV_set_ID(MSGSERV *p, MSGSERVID *p_id) {
	p->msgserv_id = p_id;
}

MSGSERVID* MSGSERV_get_ID(MSGSERV *p) {
	return p->msgserv_id;
}

void MSGSERV_set_siip(MSGSERV *p, struct in_addr ip) {
	p->siip = ip;
}

int MSGSERV_set_siip_dns(MSGSERV *p, char *dns) {
	struct hostent *hostptr;
	if((hostptr = gethostbyname(dns)) == NULL) return -1;

	struct in_addr *a = malloc(sizeof(struct in_addr));
	a = (struct in_addr *)hostptr->h_addr_list[0];
	MSGSERV_set_siip(p, *a);
	free(a);
	return 0;
}

struct in_addr MSGSERV_get_siip(MSGSERV *p ) {
	return p->siip;
}

char* MSGSERV_get_siip_dns(MSGSERV *p) {
	char buffer[128];
	char *str;
	if(gethostname(buffer, 128) == -1)
		return NULL;
	strcpy(str, buffer);
	return str;
}

void MSGSERV_set_sipt(MSGSERV *p, int sipt) {
	p->sipt = sipt;
}

int MSGSERV_get_sipt(MSGSERV *p) {
	return p->sipt;
}

void MSGSERV_set_m(MSGSERV *p, int m) {
	p->m = m;
}

int MSGSERV_get_m(MSGSERV *p) {
	return p->m;
}

void MSGSERV_set_r(MSGSERV *p, int r) {
	p->r = r;
}

int MSGSERV_get_r(MSGSERV *p) {
	return p->r;
}

int MSGSERV_set_name(MSGSERV *p, char *name) {
	return MSGSERVID_set_name(p->msgserv_id, name);
}

char* MSGSERV_get_name(MSGSERV *p) {
	return MSGSERVID_get_name(p->msgserv_id);
}

void MSGSERV_set_ip(MSGSERV *p, struct in_addr ip) {
	MSGSERVID_set_ip(p->msgserv_id, ip);
}

int MSGSERV_set_ip_str(MSGSERV *p, char *ip_str) {
	return MSGSERVID_set_ip_str(p->msgserv_id, ip_str);
}

struct in_addr MSGSERV_get_ip(MSGSERV *p) {
	return MSGSERVID_get_ip(p->msgserv_id);
}

char* MSGSERV_get_ip_str(MSGSERV *p) {
	return MSGSERVID_get_ip_str(p->msgserv_id);
}

void MSGSERV_set_upt(MSGSERV *p, int upt) {
	MSGSERVID_set_upt(p->msgserv_id, upt);
}

int MSGSERV_get_upt(MSGSERV *p) {
	return MSGSERVID_get_upt(p->msgserv_id);
}

int MSGSERV_get_upt_str(MSGSERV *p, char *str) {
	return MSGSERVID_get_upt_str(p->msgserv_id, str);
}

void MSGSERV_set_tpt(MSGSERV *p, int tpt) { 
	MSGSERVID_set_tpt(p->msgserv_id, tpt);
}

int MSGSERV_get_tpt(MSGSERV *p) {
	return MSGSERVID_get_tpt(p->msgserv_id);
}

int MSGSERV_get_tpt_str(MSGSERV *p , char *str) {
	return MSGSERVID_get_tpt_str(p->msgserv_id, str);
}