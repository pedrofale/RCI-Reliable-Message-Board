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

struct sMESSAGE {
	int lc;
	char *msg;
};

struct sMSGSERV {
	MSGSERVID *msgserv_id;
	struct in_addr siip;
	int sipt;
	int m;
	int r;
	int lc;
	MESSAGE *messages;
};

MSGSERV* MSGSERV_create(int max_msgs) {
	MSGSERV *ptr = malloc(sizeof(MSGSERV));
	ptr->messages = MESSAGES_create(max_msgs);
	ptr->msgserv_id = MSGSERVID_create();
	return ptr;
}

void MSGSERV_free(MSGSERV *p) {
	MSGSERVID_free(p->msgserv_id);
	MESSAGES_free(p);
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

int MSGSERV_set_siip_str(MSGSERV *p, char *siip_str) {
	struct in_addr *a = malloc(sizeof(struct in_addr));
	if (inet_aton(siip_str, a) == 0) { free(a); return -1; }
	MSGSERV_set_siip(p, *a);
	free(a);
	return 0;
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

char* MSGSERV_get_siip_str(MSGSERV *p) {
	return inet_ntoa(p->siip);
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

void MSGSERV_set_lc(MSGSERV *p, int lc) {
	p->lc = lc;
}

void MSGSERV_increment_lc(MSGSERV *p) {
	p->lc = p->lc + 1;
}

int MSGSERV_get_lc(MSGSERV *p) {
	return p->lc;
}


/****************************************************
 * MESSAGE methods
 ****************************************************/

MESSAGE* MESSAGES_create(int max_msgs) {
	MESSAGE *messages = malloc(max_msgs*sizeof(MESSAGE));
	for(int i = 0; i < max_msgs; i++) {
		messages[i].msg = malloc(MAX_MSG_LEN);
		strcpy(messages[i].msg, "");
		messages[i].lc = 0;
	}

	return messages;
}

int MESSAGE_set_message_str(MESSAGE message, char* msgstr) {
	int err = 0;

	if(strcpy(message.msg, msgstr) == -1)
		err = -1;

	return err;
}

char* MESSAGE_get_message_str(MESSAGE message) {
	return message.msg;
}

void MESSAGE_set_message_lc(MESSAGE message, int lc) {
	message.lc = lc;
}

int MESSAGE_get_message_lc(MESSAGE message) {
	return message.lc;
}

char* MSGSERV_get_message_str(MSGSERV *msgserv, int i) {
	return MESSAGE_get_message_str(msgserv->messages[i]);
}

int MSGSERV_set_message_str(MSGSERV *msgserv, char *msgstr, int i) {
	return MESSAGE_set_message_str(msgserv->messages[i], msgstr);
}

int MSGSERV_get_message_lc(MSGSERV *msgserv, int i) {
	return MESSAGE_get_message_lc(msgserv->messages[i]);
}

void MSGSERV_set_message_lc(MSGSERV *msgserv, int lc, int i) {
	msgserv->messages[i].lc = lc;
}

MESSAGE MSGSERV_get_message(MSGSERV *msgserv, int i) {
	return msgserv->messages[i];
}

int MSGSERV_set_message(MSGSERV *msgserv, MESSAGE message, int i) {
	int err = 0;
	err = MSGSERV_set_message_str(msgserv, message.msg, i);
	MSGSERV_set_message_lc(msgserv,  message.lc, i);

	return err;
}

int MSGSERV_get_num_messages(MSGSERV *msgserv) {
	int cnt = 0;
	int max_index = msgserv->m - 1;

	for(int i = 0; i <= max_index; i++) {
		if(strcmp(msgserv->messages[i].msg, "")) // is not empty
			cnt ++;
	}

	return cnt;
}

int MSGSERV_get_oldest_message_index(MSGSERV *msgserv) {
	int min_lc_index = 0;
	int max_index = msgserv->m - 1;

	for(int i = 1; i <= max_index; i++) {
		if(msgserv->messages[i].lc <= msgserv->messages[min_lc_index].lc) 
			min_lc_index = i;
	}

	return min_lc_index;
}

int MSGSERV_get_latest_message_index(MSGSERV *msgserv) {
	int max_lc_index = 0;
	int max_index = msgserv->m - 1;

	for(int i = 1; i <= max_index; i++) {
		if(msgserv->messages[i].lc >= msgserv->messages[max_lc_index].lc)
			max_lc_index = i;
	}

	return max_lc_index;
}

int MSGSERV_get_nth_latest_index(MSGSERV *msgserv, int n) {
	int max_index = msgserv->m - 1;
	int latest = MSGSERV_get_latest_message_index(msgserv);
	int nth_latest = latest - (n-1);

	if(nth_latest < 0)
		nth_latest = max_index - n;

	return nth_latest;
}

// returns index of oldest message if array is full
int MSGSERV_get_first_free_message_index(MSGSERV *msgserv) {
	int max_index = msgserv->m -1;
	int i = 0;

	for(i = 0; i <= max_index; i++) {
		if(!strcmp(msgserv->messages[i].msg, "")) // is empty
			break;
	}

	if(i == max_index && !strcmp(msgserv->messages[i].msg, "")) // is empty
		i = MSGSERV_get_oldest_message_index(msgserv);

	return i;
}

int MSGSERV_add_message(MSGSERV *msgserv, MESSAGE message) {
	return MSGSERV_set_message(msgserv, message, MSGSERV_get_first_free_message_index(msgserv));
}

int MSGSERV_add_message_str_lc(MSGSERV *msgserv, char *msg, int lc) {
	int err = 0;
	int i = MSGSERV_get_first_free_message_index(msgserv);
	
	err = MSGSERV_set_message_str(msgserv, msg, i);
	MSGSERV_set_message_lc(msgserv, lc, i);

	return err;
}

void MESSAGES_free(MSGSERV *msgserv) {
	for(int i = 0; i < msgserv->m; i++)
		free(msgserv->messages[i].msg);
	
	free(msgserv->messages);
}

/****************************************************
 * MSGSERVID methods
 ****************************************************/

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