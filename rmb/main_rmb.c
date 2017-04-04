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

RMB* rmb;

void parse_args(char **, int, char **);

void set_timeout(struct timeval *, int, int);

int main(int argc, char *argv[]) {
	SOCKET *idserv_socket;
	SOCKET *msgserv_socket;
	fd_set rfds;
	
	int counter;
	int maxfd=0;
	
	char server_list_str[BUFFSIZE];
	char user_input[BUFFSIZE];
	char params[2];

	struct timeval *timeout;
	time_t start;
	int diff = 0;

	parse_args(argv,argc,params);
	init_rmb(params);

	// server refresh time
	timeout = malloc(sizeof(struct timeval));
	set_timeout(timeout, REFRESH_SECS, 0);

	// Get servers from ID server and create a UDP socket to talk to one of the MSG servers
	strcpy(server_list_str, "");
	idserv_socket = create_udp_client_socket(RMB_get_siip(rmb), RMB_get_sipt(rmb));
	if(COMMRMB_get_servers(idserv_socket, server_list_str, sizeof(server_list_str)) < 0) 
		fprintf(stderr, "Failed to get server list from ID server.\n");
	MSGSERVID *msgserv_id = MSGSERVID_create();
	set_msgserv_id_str(msgserv_id, server_list_str);
	msgserv_socket = create_udp_client_socket(MSGSERVID_get_ip(msgserv_id), MSGSERVID_get_upt(msgserv_id));
	
	int fd_msgserv_socket = SOCKET_get_fd(msgserv_socket);

	// Main loop
	while(1) {
		FD_ZERO(&rfds);
		FD_SET(STDIN, &rfds);
		FD_SET(fd_msgserv_socket, &rfds);

		maxfd = fd_msgserv_socket;
		
		start = time(NULL);
		if((counter = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, timeout)) < 0) {
			printf("error: %s\n", strerror(errno));
			break;
		} else if(counter == 0) { // timeout activated: check if MSG server is still available
			strcpy(server_list_str, "");
			if(COMMRMB_get_servers(idserv_socket, server_list_str, sizeof(server_list_str)) < 0) 
				fprintf(stderr, "Failed to get server list from ID server.\n");
			if(!server_online(msgserv_id, server_list_str, strlen(server_list_str))){
				// create new socket for the first MSG server in the ID server list
				set_msgserv_id_str(msgserv_id, server_list_str);
				SOCKET_close(msgserv_socket);
				msgserv_socket = create_udp_client_socket(MSGSERVID_get_ip(msgserv_id), MSGSERVID_get_upt(msgserv_id));
			}
			set_timeout(timeout, REFRESH_SECS, 0);
		} else { // account for the time select() was blocked
			diff = time(NULL) - start;
			set_timeout(timeout, REFRESH_SECS, diff);
		}

		if(FD_ISSET(fd_msgserv_socket, &rfds)){
			COMMRMB_read_messages(msgserv_socket);
		}

		if(FD_ISSET(STDIN, &rfds)) {
			fgets(user_input, BUFFSIZE, stdin);
			if(!strncmp(user_input, "publish", strlen("publish"))) RMBUI_publish(msgserv_socket, user_input);
			if(!strcmp(user_input, "show_servers\n")) RMBUI_show_servers(idserv_socket); 
			if(!strncmp(user_input, "show_latest_messages", strlen("show_latest_messages"))) RMBUI_show_n_messages(msgserv_socket, user_input);
			if(!strcmp(user_input, "exit\n")) break;
		} 
	}

	SOCKET_close(msgserv_socket);
	MSGSERVID_free(msgserv_id);
	SOCKET_close(idserv_socket);
	free(timeout);
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

void set_timeout(struct timeval *timeout, int seconds, int diff) {
	timeout->tv_sec = seconds - diff;
	timeout->tv_usec = 0;
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

 int set_msgserv_id_str(MSGSERVID *msgservid, char *server_list){
 	char dummy[BUFFSIZE] = "";
 	char ip[BUFFSIZE]  = "";
 	char uptstr[BUFFSIZE]  = "";
 	int upt;

 	sscanf(server_list, "%[^';'];%[^';'];%[^';']", dummy, ip, uptstr);
 	upt = atoi(uptstr);
 	if(MSGSERVID_set_ip_str(msgservid, ip)==-1) 
 		return -1;
 	MSGSERVID_set_upt(msgservid, upt);

 	return 0;
 }