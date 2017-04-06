/******************************************************************************
 * File Name: rmb.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     rmb - program that publishes and accesses messages from a terminal
 *
 * USAGE
 *		Usage: rmb [-i siip] [-p sipt]
 *
 * COMMAND LINE INTERFACE
 *	 	show_servers: list the identities of all the MSG servers registered
 *						in the ID server
 *		publish message: publish a message in a MSG server
 *		show_latest_messages n: ask for the latest n messages in a MSG server
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "msgservid.h"
#include "rmb.h"
#include "rmbui.h"
#include "comm_rmb.h"
#include "comm_utils.h"

#define BUFFSIZE 500
#define REFRESH_SECS 10
#define STDIN 0

#define max(A,B) ((A)>=(B)?(A):(B))

RMB* rmb;

void parse_args(char **, int, char **);

void init_rmb(char **params);

int main(int argc, char *argv[]) {
	SOCKET *idserv_socket;
	SOCKET *msgserv_socket;
	fd_set rfds;
	
	int counter;
	int maxfd=0;
	int err = 0;
	int fd_msgserv_socket = -1;
	
	char server_list_str[BUFFSIZE];
	char messages[BUFFSIZE];
	char user_input[BUFFSIZE];
	char params[2];

	parse_args(argv,argc,params);
	init_rmb(params);

	strcpy(server_list_str, "");
	idserv_socket = create_udp_client_socket(RMB_get_siip(rmb), RMB_get_sipt(rmb));
	
	// Get servers from ID server and create a UDP socket to talk to one of the MSG servers
	err = COMMRMB_get_servers(idserv_socket, server_list_str, BUFFSIZE, UDP_NUM_TRIES);
	if(err < 0) { // couldn't fetch the list of servers from the ID server
		msgserv_socket = NULL;
		fprintf(stderr, "Couldn't reach ID server.\n");
	}
	else { 
		err = 0; 
		// connect to one of the servers in the list of servers
		msgserv_socket = COMMRMB_connect_to_message_server(idserv_socket, server_list_str); 
	}

	if(msgserv_socket != NULL) { // if there are servers available in the list
		fd_msgserv_socket = SOCKET_get_fd(msgserv_socket);
	} else {
		fprintf(stderr, "No servers available. Try again later.\n");
		err = -1;
	}

	// Main loop
	while(err == 0) {
		FD_ZERO(&rfds);
		FD_SET(STDIN, &rfds);
		if(msgserv_socket != NULL) {
			fd_msgserv_socket = SOCKET_get_fd(msgserv_socket);
			FD_SET(fd_msgserv_socket, &rfds);
		} else fd_msgserv_socket = -1;

		maxfd = max(fd_msgserv_socket, STDIN);
		if((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (fd_set*)NULL)) < 0) {
			printf("error: %s\n", strerror(errno));
			break;
		} 

		// MSG server handling
		if(FD_ISSET(fd_msgserv_socket, &rfds)){
			COMMRMB_read_messages(msgserv_socket, messages, sizeof(messages));
 			printf("%s", messages);
		}

		// UI handling
		if(FD_ISSET(STDIN, &rfds)) {
			fgets(user_input, BUFFSIZE, stdin);
			if(!strncmp(user_input, "publish", strlen("publish"))) {
				err = RMBUI_publish(msgserv_socket, user_input, UDP_NUM_TRIES); 
				if(err == 1 || err == 3) { // couldn't reach message server
					SOCKET_close(msgserv_socket);
					if(COMMRMB_get_servers(idserv_socket, server_list_str, BUFFSIZE, UDP_NUM_TRIES)> 0)
						msgserv_socket = COMMRMB_connect_to_message_server(idserv_socket, server_list_str);
					else {
						fprintf(stderr, "Couldn't reach ID server.\n");
						break;
					}
					fprintf(stdout, "Couldn't send message to server. Try again.\n");
				} else if(err == 2) { // there is no connection to a message server
					if(COMMRMB_get_servers(idserv_socket, server_list_str, BUFFSIZE, UDP_NUM_TRIES)> 0)
						msgserv_socket = COMMRMB_connect_to_message_server(idserv_socket, server_list_str);
					else {
						fprintf(stderr, "Couldn't reach ID server.\n");
						break;
					}
					if(msgserv_socket == NULL)
						fprintf(stderr, "No servers available. Try again later.\n");
					else RMBUI_publish(msgserv_socket, user_input, UDP_NUM_TRIES);
				}
				err = 0;
			}
			if(!strcmp(user_input, "show_servers\n")) {
				if(RMBUI_show_servers(idserv_socket, UDP_NUM_TRIES) < 0) break;
			}
			if(!strncmp(user_input, "show_latest_messages", strlen("show_latest_messages"))) {
				err = RMBUI_show_n_messages(msgserv_socket, user_input, UDP_NUM_TRIES); 
				if(err == 1) { // couldn't reach message server
					SOCKET_close(msgserv_socket);
					if(COMMRMB_get_servers(idserv_socket, server_list_str, BUFFSIZE, UDP_NUM_TRIES)> 0)
						msgserv_socket = COMMRMB_connect_to_message_server(idserv_socket, server_list_str);
					else {
						fprintf(stderr, "Couldn't reach ID server.\n");
						break;
					}
					fprintf(stdout, "Couldn't get messages from server. Try again.\n");
				} else if(err == 2) { // there is no connection to a message server
					if(COMMRMB_get_servers(idserv_socket, server_list_str, BUFFSIZE, UDP_NUM_TRIES)> 0)
						msgserv_socket = COMMRMB_connect_to_message_server(idserv_socket, server_list_str);
					else {
						fprintf(stderr, "Couldn't reach ID server.\n");
						break;
					}
					if(msgserv_socket == NULL)
						fprintf(stderr, "No servers available. Try again later.\n");
					else RMBUI_show_n_messages(msgserv_socket, user_input, UDP_NUM_TRIES);
				}
				err = 0;
			}
			if(!strcmp(user_input, "exit\n")) break;
		} 
	}

	SOCKET_close(msgserv_socket);
	SOCKET_close(idserv_socket);
	RMB_free(rmb);
}


 void parse_args(char **argv, int argc, char **params) {
	if (argc > 5) {
	    usage(argv[0]);
		exit(0);
	}

	params[0] = "193.136.138.142"; // IP address of tejo.tecnico.ulisboa.pt
	params[1] = "59000"; // echo server is at port 58000

	int option = 0;

	while((option = getopt(argc, argv, "i:p:")) != -1) {
		switch(option) {
			case 'i': params[0] = optarg; break;
			case 'p': params[1] = optarg; break;
			default: usage(argv[0]); exit(0);
		}
	} 
}

void usage(char *prog_name) {
  fprintf(stdout, "Usage: %s [-i siip] [-p sipt]\n", prog_name);
  return;
}

void init_rmb(char **params){
	char* p;
	rmb = RMB_create();
	RMB_set_siip_str(rmb, params[0]);
	RMB_set_sipt(rmb, (int)strtol(params[1], &p, 10));

}

int server_online(MSGSERVID *msgserv_id, char* server_list, int server_list_len){
	int status = 0;
	char dummy[128] = "";
	char ip[128]  = "";
	char tptstr[128]  = "";
	char uptstr[128] = "";
	int upt;
	char* actual_ip = MSGSERVID_get_ip_str(msgserv_id);
	int actual_upt = MSGSERVID_get_upt(msgserv_id);
	char * curr_line = server_list;
	int cnt = 0;
	int num_msgservs = 0;
  	while(curr_line && status == 0) {
  		char *next_line = strchr(curr_line, '\n');
  		int curr_line_len = next_line ? (next_line-curr_line) : strlen(curr_line);
  		char *temp_str = malloc(curr_line_len + 1);
  		if(cnt > 0) {
  			num_msgservs++;
	  		if(temp_str) {
	  			memcpy(temp_str, curr_line, curr_line_len);

	  			// temp_str contains name;ip;upt;tpt\0
	  			temp_str[curr_line_len] = '\0';
	  			if(strstr(temp_str, ";") != NULL) {
		  			// read in the IP address and TCP port of the MSG server in current line
					sscanf(temp_str, "%[^';'];%[^';'];%[^';'];%[^';']", dummy, ip, uptstr, tptstr);
 					upt = atoi(uptstr);		 
		  			if(strcmp(ip,actual_ip)==0)
		  				if(upt == actual_upt){
		  					status= 1;
		  				}
		  					
	  			}
	  		}
	  		else fprintf(stderr, "Error: malloc() failed\n");
	  	}
	  	free(temp_str);
  		cnt++;
      	curr_line = next_line ? (next_line + 1) : NULL;
  	}
  	return status;
}