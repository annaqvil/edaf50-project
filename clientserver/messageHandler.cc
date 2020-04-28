#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include "connection.h"
#include "protocol.h"
#include "messageHandler.h"

using std::endl;
using std::string; 
using std::cout; 
using std::cin; 
using std::cerr;
using std::stoi;
using std::exception;


MessageHandler::MessageHandler(std::shared_ptr<Connection>& c): conn(c) {}


void MessageHandler::sendByte(const char code){
	try{
		conn->write(code); 
	} catch (exception& e){
		cerr << "Could not send byte" << e.what() << endl; 
	}
}


void MessageHandler::sendCode(Protocol code){
	sendByte(static_cast<const char>(code)); 
}

void MessageHandler::sendInt(int value){
	try {
		conn->write((value >> 24) & 0xFF);
        conn->write((value >> 16) & 0xFF);
        conn->write((value >> 8) & 0xFF);
        conn->write(value & 0xFF);
	} catch (exception& e){
		cerr << "Could not send int " << e.what() << endl; 
	}
}

void MessageHandler::sendIntParameter(int param){
	sendCode(Protocol::PAR_NUM);
	sendInt(param);
}

void MessageHandler::sendStringParameter(string param){
	sendCode(Protocol::PAR_STRING);
	sendInt(param.length());
	for (unsigned int i = 0; i < param.length(); i++){
		sendByte(param.at(i));
	}
}

unsigned char MessageHandler::recvByte(){
	int code = 0;
	try{
		code = conn->read(); 
	}  catch (exception& e){
		cerr << "Could not read " << e.what() << endl; 
	}
	return code; 
}

Protocol MessageHandler::recvCommand(){
	Protocol command = Protocol::UNDEFINED; 
	try{
		command = static_cast<const Protocol>(conn->read()); 
	} catch (exception& e){
		cerr << "Could not read command " << e.what() << endl; 
	}
	return command;
}

int MessageHandler::recvInt(){
	int b1 = recvByte();
	int b2 = recvByte();
	int b3 = recvByte();
	int b4 = recvByte();
	return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int MessageHandler::recvIntParameter(){
    if(recvCommand() != Protocol::PAR_NUM){
        cerr << "Wrong parameter"; 
    }
    return recvInt();
}

string MessageHandler::recvStringParameter(){
	Protocol code = recvCommand();
	if (code != Protocol::PAR_STRING) {
		cerr << "Wrong parameter"; 
	}
	int n = recvInt();
	if (n < 0) {
		cerr << "Number of characters < 0";
	}
	string result;
	try{
		for (int i = 0; i < n; i++) {
			result.push_back(conn->read());
		}
	} catch (exception& e){
		cerr << "Could not read" << e.what() << endl;
	}

	return result;
}




