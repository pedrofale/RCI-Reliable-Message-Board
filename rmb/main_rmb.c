/******************************************************************************
 * File Name: rmb.c
 * Author: Filipe Ferreira, João Nobre, Pedro Ferreira IST MEEC
 *
 * NAME
 *     rmb - program that publishes and accesses messages from a terminal     TRADUÇAO ????????
 *
 * DESCRIPTION
 *      
 *
 * USAGE
 *		"Usage: rmb [-i siip] [-p sipt]
 *
 * COMMAND LINE INTERFACE
 *	
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "msgserv.h"
#include "msgservui.h"
#include "comm_utils.h"

void parse_args(char **, int, char **);

int main(int argc, char *argv[]) {


}


 void parse_args(char **argv, int argc, char **params) {
	if (argc < 9 || argc > 17) {
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