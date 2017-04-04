/******************************************************************************
 * File Name: msgserv.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     msgserv - message server for the RCI reliable message board app
 *
 * DESCRIPTION
 *      Program to run on each message server which contacts the ID server via UDP,
 *		the terminals via UDP and the other message servers via TCP.
 *
 * USAGE
 *		"Usage: msgserv -n name -j ip -u upt -t tpt [-i siip] [-p sipt] [-m m] [-r r]
 *
 * COMMAND LINE INTERFACE
 *		join: register the server in the ID server
 *		show_servers: list the identities of all the other MSG servers this one
 *					  has a TCP session estabilished with
 *		show_messages: list all the messages this server has stored, ordered by
 *					   their LCs
 *		exit: exit the app
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "msgserv.h"
#include "msgservui.h"
#include "comm_utils.h"
#include <errno.h>

#define BUFFSIZE 500
#define STDIN 0

MSGSERV *msgserv;

void usage(char *);

void init_msgserv(char **);

void print_id();

void parse_args(char **, int, char **);

void set_timeout(struct timeval *, int, int);

int main(int argc, char *argv[]) {
	SOCKET *idserv_clientsocket;
	SOCKET *terminal_serversocket;
	SOCKET *msgserv_serversocket;

	SOCKET **msgserv_socketarray = NULL;
	int *fd_msgserv_client = 0;
	
	fd_set rfds;

	int i;
	int counter;
	int maxfd = 0;
	int fd_idserv_client, fd_terminal_server, fd_msgserv_server;
	int num_msgservs = 0;
	int err = 0;

	struct timeval* timeout;
	time_t t1;
	int diff = 0;

	char user_input[BUFFSIZE] = "";
	char server_list[BUFFSIZE] = "";
	int new_message_lc;

	// protect against SIGPIPE signals caused by TCP sessions
	void (*old_handler)(int);
	if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR) exit(1); // error
	
	// array of char* containing each parameter string for the MSG server
	char *params[8];

	parse_args(argv, argc, params);
	init_msgserv(params);

	// server refresh time
	timeout = malloc(sizeof(struct timeval));
	set_timeout(timeout, MSGSERV_get_r(msgserv), 0);

	/* create UDP server socket on port upt to connect to RMB terminals */
	terminal_serversocket = create_udp_server_socket(MSGSERV_get_upt(msgserv));
	if(terminal_serversocket == NULL) err = -1;

	/* create UDP client socket to connect to ID server */
	idserv_clientsocket = create_udp_client_socket(MSGSERV_get_siip(msgserv), MSGSERV_get_sipt(msgserv));
	if(idserv_clientsocket == NULL) err = -1;

	/* create TCP server on port tpt */
	msgserv_serversocket = create_tcp_server_socket(MSGSERV_get_tpt(msgserv));
	if(msgserv_serversocket == NULL) err = -1;

	/* register this message server in the ID server */
	MSGSERVUI_join(msgserv, idserv_clientsocket);

	/* count number of MSG servers registered in the ID server */
	num_msgservs = COMMMSGSERV_get_server_list(idserv_clientsocket, server_list, sizeof(server_list));
	
	if(num_msgservs > 0) {
		msgserv_socketarray = malloc(num_msgservs*sizeof(SOCKET*));
		fd_msgserv_client = malloc(num_msgservs*sizeof(int));
		/* estabilish TCP session with all MSG servers available in the ID server */
		COMMMSGSERV_connect_msgservs(msgserv_socketarray, server_list, msgserv);
		/* get all the messages from another MSG server (eg the first on the list) */
		COMMMSGSERV_request_messages_for_msgserv(msgserv_socketarray, num_msgservs, msgserv);
	}

	if(idserv_clientsocket != NULL)
		fd_idserv_client = SOCKET_get_fd(idserv_clientsocket);
	if(terminal_serversocket != NULL)
		fd_terminal_server = SOCKET_get_fd(terminal_serversocket);
	if(msgserv_serversocket != NULL)
		fd_msgserv_server = SOCKET_get_fd(msgserv_serversocket);

	// Main loop
	while(err == 0) {
		FD_ZERO(&rfds);
		FD_SET(STDIN, &rfds);
		FD_SET(fd_terminal_server, &rfds);
		FD_SET(fd_msgserv_server, &rfds);
		
		if (fd_terminal_server > maxfd)
			maxfd = fd_terminal_server;
		if (fd_msgserv_server > maxfd)
			maxfd = fd_msgserv_server;

		for(i = 0; i < num_msgservs; i++) {
			if(msgserv_socketarray[i] != NULL) {
				fd_msgserv_client[i] = SOCKET_get_fd(msgserv_socketarray[i]);
				if(fd_msgserv_client[i] > maxfd)
					maxfd = fd_msgserv_client[i];

				FD_SET(fd_msgserv_client[i], &rfds);
			}
		}

		t1 = time(NULL);
		if((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, timeout)) < 0) {
			fprintf(stderr, "error: %s\n", strerror(errno));
			break;
		} else if(counter == 0) { // timeout activated
			MSGSERVUI_join(msgserv, idserv_clientsocket);
			set_timeout(timeout, MSGSERV_get_r(msgserv), 0);
		} else { // account for the time select() was blocked
			diff = time(NULL) - t1;
			set_timeout(timeout, MSGSERV_get_r(msgserv), diff);
		}

		// UI handling
		if(FD_ISSET(STDIN, &rfds)) {
			fgets(user_input, BUFFSIZE, stdin);
			if(!strcmp(user_input, "join\n")) MSGSERVUI_join(msgserv, idserv_clientsocket);
			if(!strcmp(user_input, "show_servers\n")) MSGSERVUI_show_servers(idserv_clientsocket);
			if(!strcmp(user_input, "show_messages\n")) MSGSERVUI_show_messages(msgserv);
			if(!strcmp(user_input, "exit\n")) { break; }
		}

		// Terminal handling
		if(FD_ISSET(fd_terminal_server, &rfds)) {
			if(COMMMSGSERV_read_from_terminal(terminal_serversocket, msgserv, msgserv_socketarray, num_msgservs) < 0) {
				fprintf(stderr, "Error reading from terminal socket\n");
			}
		}

		// Message servers handling
		for(i = 0; i < num_msgservs; i++) {
			if(msgserv_socketarray[i] != NULL)
				if(FD_ISSET(fd_msgserv_client[i], &rfds)) {
					if((err = COMMMSGSERV_read_from_msgserv(msgserv_socketarray[i], msgserv)) == -1) {
						fprintf(stderr, "Error reading from message server socket\n");
						err = 0;
					}
						
					if(err == -2) { // connection closed by peer: close this socket
						SOCKET_close(msgserv_socketarray[i]);
						msgserv_socketarray[i] = NULL;
						fd_msgserv_client[i] = -1;
						err = 0;
					}
				}
		}

		// Message server session keeping
		if(FD_ISSET(fd_msgserv_server, &rfds)) {
			num_msgservs++;

			if(num_msgservs - 1 == 0) {
				msgserv_socketarray = malloc(num_msgservs*sizeof(SOCKET*));
				fd_msgserv_client = malloc(num_msgservs*sizeof(int));
			}
			else { // can only realloc() if already malloc()ed
				msgserv_socketarray = realloc(msgserv_socketarray, num_msgservs * sizeof(SOCKET*));
				fd_msgserv_client = realloc(fd_msgserv_client, num_msgservs * sizeof(int));
			}

			if(msgserv_socketarray == NULL || fd_msgserv_client == NULL) {
				fprintf(stderr, "Error allocating memory\n");
				break;
			}

			// add this TCP socket to the array of sockets of message servers
			msgserv_socketarray[num_msgservs - 1] = accept_tcp_server_socket(msgserv_serversocket);
		}
	} 

	if(num_msgservs > 0) {
		free(fd_msgserv_client);
		for(i = 0; i < num_msgservs; i++) {
			SOCKET_close(msgserv_socketarray[i]);
		}
		free(msgserv_socketarray);
	}

	SOCKET_close(msgserv_serversocket);
	SOCKET_close(idserv_clientsocket);
	SOCKET_close(terminal_serversocket);
	free(timeout);
	MSGSERV_free(msgserv);
	
	exit(0);
}

void parse_args(char **argv, int argc, char **params) {
	if (argc < 9 || argc > 17) {
	    usage(argv[0]);
		exit(0);
	}

	params[4] = "193.136.138.142"; // IP address of tejo.tecnico.ulisboa.pt
	params[5] = "59000"; // echo server is at port 58000
	params[6] = "200";
	params[7] = "10";

	int option = 0;

	while((option = getopt(argc, argv, "n:j:u:t:i:p:m:r:")) != -1) {
		switch(option) {
			case 'n': params[0] = optarg; break;
			case 'j': params[1] = optarg; break;
			case 'u': params[2] = optarg; break;
			case 't': params[3] = optarg; break;
			case 'i': params[4] = optarg; break;
			case 'p': params[5] = optarg; break;
			case 'm': params[6] = optarg; break;
			case 'r': params[7] = optarg; break;
			default: usage(argv[0]); exit(0);
		}
	} 

	if (params[0] == "" || params[1] == "" || params[2] == "" || params[3] == "") {
		usage(argv[0]);
		exit(0);
	}
}

void usage(char *prog_name) {
  fprintf(stdout, "Usage: %s -n name -j ip -u upt -t tpt [-i siip] [-p sipt] [-m m] [-r r]\n", prog_name);
  return;
}

void init_msgserv(char **params) {
	char *p;
	int m = (int)strtol(params[6], &p, 10);
	if(m <= 0) {
		fprintf(stderr, "Error: parameter m must be greater than 0.\n");
		exit(-1);
	}
	msgserv = MSGSERV_create(m);

	if(MSGSERV_set_name(msgserv, params[0]) == -1) {
		fprintf(stderr, "Error setting MSGSERVID name.\n");
		exit(-1);
	}
	if(MSGSERV_set_ip_str(msgserv, params[1]) == -1) {
		fprintf(stderr, "Error setting MSGSERVID IP address from string.\n");
		exit(-1);
	}
	MSGSERV_set_upt(msgserv, (int)strtol(params[2], &p, 10));
	MSGSERV_set_tpt(msgserv, (int)strtol(params[3], &p, 10));

	if(MSGSERV_set_siip_str(msgserv, params[4]) == -1) {
		fprintf(stderr, "Error setting MSGSERV SIIP address from string.\n");
		exit(-1);
	}

	MSGSERV_set_sipt(msgserv, (int)strtol(params[5], &p, 10));
	MSGSERV_set_m(msgserv, m);
	MSGSERV_set_r(msgserv, (int)strtol(params[7], &p, 10));
}

void set_timeout(struct timeval *timeout, int seconds, int diff) {
	timeout->tv_sec = seconds - diff;
	timeout->tv_usec = 0;
}

void print_id() {
	printf("name: %s\n", MSGSERV_get_name(msgserv));
	printf("ip: %s\n", MSGSERV_get_ip_str(msgserv));
	printf("upt: %d\n", MSGSERV_get_upt(msgserv));
	printf("tpt: %d\n", MSGSERV_get_tpt(msgserv));
	printf("siip: %s\n", MSGSERV_get_siip_str(msgserv));
	printf("sipt: %d\n", MSGSERV_get_sipt(msgserv));
	printf("m: %d\n", MSGSERV_get_m(msgserv));
	printf("r: %d\n", MSGSERV_get_r(msgserv));
}