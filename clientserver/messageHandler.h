#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <string>

class MessageHandler {
	public:
	MessageHandler(Connection& c); //borde detta vara något annat typ pointer? Shared pointer?
	void sendCode(Protocol code);
	void sendInt(int value);
	void sendIntParameter(int param);
	void sendStringParameter(std::string param);
	Protocol recvCommand();
	int recvInt();
	int recvIntParameter();
	std::string recvStringParameter();
private:
	Connection& conn;
	unsigned char recvByte();
  	void sendByte(const char code);
};

#endif