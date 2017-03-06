#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

typedef struct sSOCKET SOCKET;

void SOCKET_set_fd(SOCKET *socket, int fd);

int SOCKET_get_fd(SOCKET *socket);

void SOCKET_set_addr(SOCKET *socket, struct sockaddr_in addr);

struct sockaddr_in SOCKET_get_addr(SOCKET *socket);

SOCKET* create_udp_socket(struct in_addr ip, int port);

int close_udp_socket(SOCKET *socket);

int sendmsg_udp(SOCKET *socket, char *msg, int);

int readmsg_udp(SOCKET *socket, char *msg, int);