#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include "connection.h"
#include "protocol.h"

using std::endl;
using std::string; 
using std::cout; 
using std::cin; 
using std::cerr;
using std::stoi;
using std::exception;


void sendByte(Connection& conn, int code){
	try{
		conn.write(static_cast<const char>(code)); 
	} catch (exception& e){
		cerr << "Could not send byte" << e.what() << endl; 
	}
}


void sendCode(Connection& conn, Protocol code){
	sendByte(conn, static_cast<const char>(code)); 
}

void sendInt(Connection& conn, int value){
	try {
		conn.write((value >> 24) & 0xFF);
        conn.write((value >> 16) & 0xFF);
        conn.write((value >> 8) & 0xFF);
        conn.write(value & 0xFF);
	} catch (exception& e){
		cerr << "Could not send int " << e.what() << endl; 
	}
}

void sendIntParameter(Connection& conn, int param){
	sendCode(conn, Protocol::PAR_NUM);
	sendInt(conn, param);
}

void sendStringParameter(Connection& conn, string param){
	sendCode(conn, Protocol::PAR_STRING);
	sendInt(conn, param.length());
	for (unsigned int i = 0; i < param.length(); i++){
		sendByte(conn, param.at(i));
	}
}

int recvByte(Connection& conn){
	int code = 0;
	try{
		code = conn.read(); 
	}  catch (exception& e){
		cerr << "Could not read " << e.what() << endl; 
	}
	return code; 
}

Protocol recvCommand(Connection& conn){
	Protocol command = Protocol::UNDEFINED; 
	try{
		command = static_cast<const Protocol>(conn.read()); 
	} catch (exception& e){
		cerr << "Could not read command " << e.what() << endl; 
	}
	return command;
}

int recvInt(Connection& conn){
	int b1 = recvByte(conn);
	int b2 = recvByte(conn);
	int b3 = recvByte(conn);
	int b4 = recvByte(conn);
	return b1 << 24 | b2 << 16 | b3 << 8 | b4;
}

int recvIntParameter(Connection& conn){
    if(recvCommand(conn) != Protocol::PAR_NUM){
        throw "Wrong parameter"; 
    }
    return recvInt(conn);
}

string recvStringParameter(Connection& conn){
	Protocol code = recvCommand(conn);
	if (code != Protocol::PAR_STRING) {
		throw "Wrong parameter"; 
	}
	int n = recvInt(conn);
	if (n < 0) {
		throw "Number of characters < 0";
	}
	string result;
	try{
		for (int i = 0; i <= n; i++) {
			result.push_back(conn.read());
		}
	} catch (exception& e){
		cerr << "Could not read" << e.what() << endl;
	}

	return result;
}




