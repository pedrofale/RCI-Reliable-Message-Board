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

struct sMSGSERVID {
	char name[MAX_NAME_LEN];
	struct in_addr ip;
	int upt; 
	int tpt;
};

MSGSERVID* MSGSERVID_create() {
	MSGSERVID *ptr = malloc(sizeof(MSGSERVID));
	memset(ptr, 0, sizeof(MSGSERVID));
	return ptr; 
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
	struct in_addr a;
	if(inet_aton(ip_str, &a) == 0) { 
		return -1; 
	}
	MSGSERVID_set_ip(p, a);
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

int MSGSERVID_get_upt_str(MSGSERVID* p, char *str) {
	return sprintf(str, "%d", p->upt);
}

void MSGSERVID_set_tpt(MSGSERVID* p, int tpt) { 
	p->tpt = tpt;
}

int MSGSERVID_get_tpt(MSGSERVID* p) {
	return p->tpt;
}

int MSGSERVID_get_tpt_str(MSGSERVID* p, char* str) {
	return sprintf(str, "%d", p->tpt);
}

int MSGSERVID_set_ID_from_reg(MSGSERVID *p, char *reg_str) {
	char name[128] = "";
 	char ip[20] = "";
 	char uptstr[10]  = "";
 	char tptstr[10] = "";
 	int tpt;
 	int upt;
 	int err = 0;

 	sscanf(reg_str, "%[^';'];%[^';'];%[^';'];%[^';']", name, ip, uptstr, tptstr);
 	upt = atoi(uptstr);
 	tpt = atoi(tptstr);

 	MSGSERVID_set_name(p, name);
 	if(MSGSERVID_set_ip_str(p, ip) == -1) 
 		err = -1;
 	MSGSERVID_set_upt(p, upt);
 	MSGSERVID_set_tpt(p, tpt);

 	return err;
}

int MSGSERVID_compare_ID(MSGSERVID *p1, MSGSERVID *p2) {
	int are_equal = 0;

	if(strcmp(MSGSERVID_get_name(p1), MSGSERVID_get_name(p2)) == 0)
		if(strcmp(MSGSERVID_get_ip_str(p1), MSGSERVID_get_ip_str(p2)) == 0)
			if(MSGSERVID_get_upt(p1) == MSGSERVID_get_upt(p2))
				if(MSGSERVID_get_tpt(p1) == MSGSERVID_get_tpt(p2))
					are_equal = 1;

	return are_equal;
}

int MSGSERVID_compare_ip(MSGSERVID *p1, MSGSERVID *p2) {
	int are_equal = 0;

	if(strcmp(MSGSERVID_get_ip_str(p1), MSGSERVID_get_ip_str(p2)) == 0)
		are_equal = 1;

	return are_equal;
}

int MSGSERVID_compare_upt(MSGSERVID *p1, MSGSERVID *p2) {
	int are_equal = 0;

	if(MSGSERVID_get_upt(p1) == MSGSERVID_get_upt(p2))
		are_equal = 1;

	return are_equal;
}

int MSGSERVID_compare_tpt(MSGSERVID *p1, MSGSERVID *p2) {
	int are_equal = 0;

	if(MSGSERVID_get_tpt(p1) == MSGSERVID_get_tpt(p2))
		are_equal = 1;

	return are_equal;
}