#ifndef _COMMUTILS_H_
#define _COMMUTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct sSOCKET SOCKET;

void SOCKET_set_fd(SOCKET *socket, int fd);

int SOCKET_get_fd(SOCKET *socket);

void SOCKET_set_addr(SOCKET *socket, struct sockaddr_in addr);

struct sockaddr_in SOCKET_get_addr(SOCKET *socket);

int SOCKET_close(SOCKET *socket);

/****************************************************
 * UDP methods
 ****************************************************/

SOCKET* create_udp_client_socket(struct in_addr ip, int port);

SOCKET* create_udp_server_socket(int port);

int sendmsg_udp(SOCKET *socket, char *msg, int msglen);

int readmsg_udp(SOCKET *socket, char *msg, int msglen);

/****************************************************
 * TCP methods
 ****************************************************/

SOCKET* create_tcp_client_socket(struct in_addr ip, int port, int timeout);

SOCKET* create_tcp_server_socket(int port);

SOCKET* accept_tcp_server_socket(SOCKET *socket);

int writemsg_tcp(SOCKET *socket, char *msg, int);

int readmsg_tcp(SOCKET *socket, char *msg, int);

#endif