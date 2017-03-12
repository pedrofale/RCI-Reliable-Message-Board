#ifndef _COMM_MSGSERV_H_
#define _COMM_MSGSERV_H_

#include "comm_utils.h"
#include "msgserv.h"

#define MAX_BUFF_SIZE 500

#define PUBLISH_MESSAGE "PUBLISH "
#define GET_MESSAGES "GET_MESSAGES "
#define MESSAGES "MESSAGES\n"

int read_from_terminal(SOCKET *sckt, MSGSERV *msgserv);


#endif