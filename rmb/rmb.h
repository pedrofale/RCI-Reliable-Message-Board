#ifndef _RMB_H_
#define _RMB_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

typedef struct sRMB RMB;
RMB* RMB_create();
void RMB_free(RMB *rmb);
int RMB_set_siip_str(RMB *p, char *siip_str);
struct in_addr RMB_get_siip(RMB *p);
void RMB_set_sipt(RMB *p, int sipt);
int RMB_get_sipt(RMB *p);


#endif