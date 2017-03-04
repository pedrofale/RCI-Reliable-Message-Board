/******************************************************************************
 * File Name: msgserv.c
 * Author: Pedro Ferreira, João Nobre, Filipe Ferreira IST MEEC
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
#include <unistd.h>
#include "msgserv.h"

MSGSERV *msgserv;

void usage(char *);

void init_msgserv_id(char **);

void print_id();

void parse_args(char **, int, char **);

int main(int argc, char *argv[]) {
	// array of char* containing each parameter string for the MSG server
	char *params[8];

	msgserv = MSGSERV_create();
	parse_args(argv, argc, params);
	init_msgserv_id(params);
	print_id();

	/* create UDP server on port upt */


	/* create TCP server on port tpt */

	/* wait for input from the CLI */
	while(1) {
		/* main routines go here */
		break;	
	}

	MSGSERV_free(msgserv);
	exit(0);
}

void parse_args(char **argv, int argc, char **params) {
	if (argc < 9 || argc > 17) {
	    usage(argv[0]);
		exit(0);
	}

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

void init_msgserv_id(char **params) {
	char *p;

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
}

void print_id() {
	printf("name: %s\n", MSGSERV_get_name(msgserv));
	printf("ip: %s\n", MSGSERV_get_ip_str(msgserv));
	printf("upt: %d\n", MSGSERV_get_upt(msgserv));
	printf("tpt: %d\n", MSGSERV_get_tpt(msgserv));
}

