# RCI-Reliable-Message-Board
Project for the Computer Networks course at IST, Lisbon during the summer semester of 2016/17.
Authors: Filipe Ferreira, João Nobre, Pedro Ferreira


To run the ID server: `./sid64 -debug`.

To run `msgserv` and `rmb`, you need to make sure you're connecting to the right ID server. If you're running it on your machine, make sure you run `msgserv` and `rmb` with the option `-i` set to your machine's IP address. In Ubuntu you can find this out by using the command `ifconfig`.

## How it works
The project consists of two applications: `rmb` and `msgserv`. `rmb` is the app that users use to communicate with each other, publishing and accessing messages in the message board. `msgserv` is a server which stores messages. An ID server is assumed to exist, containing the list of online `msgserv`s.

When a `msgserv` is launched, it creates 3 sockets: one to listen to UDP requests from an `rmb`, another to send UDP requests to the ID server and another to listen to TCP requests from other `msgserv`s. Then, it sends a `GET_SERVERS` message via UDP to the ID server, to which the ID server responds with a `SERVERS\n(name;ip;upt;tpt)\n` (one `\n` after each server description). The `msgserv` then sends a `REG name;ip;upt;tpt` UDP message to the ID server, and the ID server adds this server to the server list. Then, the `msgserv` counts the number of connected servers. If there are other connected `msgserv`s, this server estabilishes a TCP session (`socket()` + `connect()`) with each one (avoiding a connection to itself).  

After connecting to the other servers, this `msgserv` fetches the messages which the others already have stored in their memories. This is done by sending a `SGET_MESSAGES\n` TCP message to each of the connected servers (we decided to send to all of them to check if they are actually alive). Each server will answer with a `SMESSAGES\n` TCP message containing all its messages. After all servers have been queried for messages, the response of the last one is the one used to copy the received messages into this `msgserv`.

We now enter the main loop of the `msgserv` application. The app will be constantly listening, via a `select()`, to incoming requests from the command line, from an `rmb` or from other `msgserv`s, which may be either trying to `connect()` - in which case we call `accept()` - to this one or sending a `MESSAGES\n` TCP message. We set a `timeout` in `select()` so that every `T` seconds the server sends a `REG name;ip;upt;tpt` UDP message to the ID server to signal that it's online and available.

All UDP requests, both in `msgserv` and in `rmb`, are made so as to guarantee that the message was received by the peer or, if it wasn't, to inform the requester of what happened. Everytime a UDP request is made to an ID server or from an `rmb` to a `msgserv`, we wait 1 second 5 times (yielding a maximum of 5 seconds of hold), each time repeating the request we didn't get an answer from before. In the case of `rmb` to `msgserv` requests, we send a `GET_MESSAGES n` message to see if the server is responding. If it isn't - no messages where received - after 5 tries, `rmb` asks the ID server for the server list, and randomly tries to connect to another server in the list.

If a `msgserv` tries to connect to one which is not online, `connect()` fails and that socket is set to `NULL`.
