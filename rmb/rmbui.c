/* send "GET_SERVERS" to siip at port sipt via UDP */
 int show_servers(RMB *p, SOCKET* socket) {
 	char msg[MAX_MSG_LEN];
 	char resp[MAX_MSG_LEN];
 	int err = 0;
 	strcpy(msg, "GET_SERVERS");

 	if(sendmsg_udp(socket, msg, sizeof(msg)) == -1) err = -1;
 	printf(">> Sent: %s\n", msg);

 	if(readmsg_udp(socket, resp, sizeof(msg)) == -1) err = -2;
 	printf(">> Received: %s\n", resp);

 	/* parse message from ID server to create a list of MSG servers */

 	return err;
 }



 int show_nmessages() {
 	return 0;
 }