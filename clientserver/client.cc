#include <iostream>
#include <string>
#include "connection.h"

using std::endl;
using std::string; 
using std::cout; 
using std::cin; 

void write(const Connection& conn, string ){
	// do weird shit
}

void read(const Connection& conn){
	//write shit
}

Connection init(int argc, char* argv[]){
	if(argc != 3){
		// cerr är som cout för errors
		cerr << "Usage: client host-name port-number" << endl; 
		exit(1); 
	}

	int port = -1; 
	try {
		port = stoi(argv[2]); 
	} catch (exception& e) {
		cerr << "Wrong port-number. " << e.what() << endl;
		exit(2); 		
	}

	Connection conn(argv[1], port); 
	if(!conn.isConnected()){
		cerr << "Connection attept failed" << endl; 
		exit(3);
	}

	return conn;
}

int main(int argc, char* argv[])
{
	Connection conn = init(argc, argv); 

	while(true){
		string message; 
		cin >> message;
		cout << message; 
	}
	return 0;
}