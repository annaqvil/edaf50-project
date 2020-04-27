/* ourserver.cc [communism intensifies] */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "messageHandler.h"
#include "protocol.h"
#include <memory>

using std::cerr;
using std::endl;
using std::stoi;
using std::cout;

Server init(int argc, char* argv[])
{
	if (argc != 2) {
		cerr << "Usage: ourserver port-number" << endl;
		exit(1);
	}

	int port = -1;
	try {
		port = stoi(argv[1]);
	}
	catch (std::exception & e) {
		cerr << "Wrong format for port number. " << e.what() << endl;
		exit(2);
	}

	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(3);
	}
	return server;
}

int main(int argc, char* argv[])
{
	auto server = init(argc, argv);


	while (true) {
		auto conn = server.waitForActivity();
		MessageHandler ms(conn);
		if (conn != nullptr) {
			//try {
			Protocol p = ms.recvCommand();
			switch (p) {
			case Protocol::COM_LIST_NG:
				//listNewsgroups();
				break;
				//case MEDIUM:
				//    ...
			}
		}
		else {
			conn = std::make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
	return 0;
}

int listNewsgroups() {
	return 0;
}