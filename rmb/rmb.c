


#include "rmb.h"






struct sRMB {
	struct in_addr siip;
	int sipt;
};


RMB* RMB_create(){

	return malloc(sizeof(RMB));

}

void RMB_free(RMB *rmb){

	free(rmb);

}




void RMB_set_siip(RMB *p, struct in_addr ip) {
	p->siip = ip;
}

int RMB_set_siip_str(RMB *p, char *siip_str) {
	struct in_addr *a = malloc(sizeof(struct in_addr));
	if (inet_aton(siip_str, a) == 0) { free(a); return -1; }
	RMB_set_siip(p, *a);
	free(a);
	return 0;
}



struct in_addr RMB_get_siip(RMB *p ) {
	return p->siip;
}

char* RMB_get_siip_str(RMB *p) {
	return inet_ntoa(p->siip);
}

char* RMB_get_siip_dns(RMB *p) {
	char buffer[128];
	char *str;
	if(gethostname(buffer, 128) == -1)
		return NULL;
	strcpy(str, buffer);
	return str;
}

void RMB_set_sipt(RMB *p, int sipt) {
	p->sipt = sipt;
}

int RMB_get_sipt(RMB *p) {
	return p->sipt;
}

