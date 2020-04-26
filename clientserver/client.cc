#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include "connection.h"
#include "protocol.h"
#include "messageHandler.h"
#include <vector>
#include <sstream>
#include <map>

using std::endl;
using std::string; 
using std::cout; 
using std::cin; 
using std::cerr;
using std::stoi;
using std::exception;
using std::istream; 
using std::vector; 
using std::map;
//using std::count; 


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

 //all articles have unique numbers
void printInstructions(){
	cout << "------------------------------------------------------------------------------------" << endl;
	cout<< "You can place the following commands: " << endl;
	cout << "list - lists all newsgroups" << endl <<
			"list <newsgroup-number> - lists all articles in a specifik newsgroup" << endl <<
			"read <newsgroup-number> <article-number> - let you read a specific artlice" << endl <<
			"create <newsgroup-name> - creates a new newsgroup" << endl << 
			"write - will give you the opportunity to create an article" << endl << 
			"delete <newsgroup-number> - deletes a specific newsgroup" << endl << 
			"delete <newsgroup-number> <article-number> - deletes article from specified newsgroup" << endl << 
			"exit - exits program" << endl << 
			"help - repets these instuctions" << endl; 
	cout << "------------------------------------------------------------------------------------" << endl;
}

void handleEnd(MessageHandler &ms)
{
    if (ms.recvCommand() != Protocol::ANS_END)
    {
        cerr << "Error! Protocol not ok" << endl;
    }
}

bool handleAck(MessageHandler &ms){
	if(ms.recvCommand() == Protocol::ANS_ACK){
		return true;
	} else {
		Protocol fault = ms.recvCommand();
		if(fault == Protocol::ERR_NG_ALREADY_EXISTS)
			cout << "Newsgroup already exists" << endl; 
		if(fault == Protocol::ERR_NG_DOES_NOT_EXIST)
			cout << "Newsgroup does not exist" << endl; 
		if(fault == Protocol::ERR_ART_DOES_NOT_EXIST)
			cout << "Article does not exist" << endl; 
		return false; 
	}
}

void listNewsgoups(MessageHandler& ms){
	 ms.sendCode(Protocol::COM_LIST_NG); 
	 ms.sendCode(Protocol::COM_END); 

	 int nbrOfGroups; 

	 cout << "Newsgroup-number: newsgroup-name" << endl; 

	 if(ms.recvCommand() == Protocol::ANS_LIST_NG){
	 	nbrOfGroups = ms.recvIntParameter(); 
	 	for(int i = 0; i<nbrOfGroups; i++){
	 		cout << ms.recvIntParameter() << ": " << ms.recvStringParameter() << endl; 
	 	}
	 	handleEnd(ms);
	 } else {
	 	cerr << "Error! Wrong protocol stuffies" << endl; 
	 }
}

void listArticles(int newsgroup, MessageHandler& ms){
	ms.sendCode(Protocol::COM_LIST_ART); 
	ms.sendIntParameter(newsgroup); 
	ms.sendCode(Protocol::COM_END); 

	cout << "Article-number: article-name" << endl; 
	if(ms.recvCommand() == Protocol::ANS_LIST_ART){
		if(handleAck(ms)){
			int nbrOfArticles = ms.recvIntParameter();
			for(int i = 0; i<nbrOfArticles; i++){
				cout << ms.recvIntParameter() << ": " << ms.recvStringParameter() << endl; 
			}
		}
		handleEnd(ms); 
	} else {
		cerr << "Error! Something wrong" << endl; 
	}
}

void readArticle(int newsgroup, int article){
	cout << "read article nr " << article << " in newsgroup " << newsgroup << endl; 
}

void createNewsgroup(string newsgroup){
	cout << "create newsgroup with name: " << newsgroup << endl; 
}

void writeArticle(){
	cout << "write article" << endl; 
}

void deleteNewsgroup(int newsgroup){
	cout << "delete newsgroup nr " << newsgroup << endl; 
}

void deleteArticle(int newsgroup, int article){
	cout << "delete article nr " << article << " from newsgroup " << newsgroup << endl; 
}

int main(int argc, char* argv[])
{
	Connection conn = init(argc, argv); 
	printInstructions();
	MessageHandler ms(conn); 

	while(true){
		cout << "news> ";
		//allt i följande stycke bör nog brytas ut för minneshantering 
		string line;
		std::getline(cin, line); 
		vector<string> input; 
		std::istringstream str(line);
   		std::string word;

   		while ( str >> word ){
    		input.push_back(word);
   		}

   		size_t n = input.size();

   		if(n < 1){
   			continue; 
   		}
   		string first = input.at(0); 

   		if(first == "list"){
   			if(n == 1)
   				listNewsgoups(ms);
   			else if(n == 2)
   				try{
   					int newsgroup = stoi(input.at(1)); 
   					listArticles(newsgroup, ms);
   				} catch (exception& e){
   					cout << "Wrong input, expected list <newsgroup-number>, e.g. 'list 2'" << endl; 
   				}
   			else 
   				cout << "Expected either 'list' or 'list <newsgroup-number>'. Type help for more help" <<endl; 
   		}
   		else if(first == "read"){
   			if(n == 3){
				try{
					int newsgroup = stoi(input.at(1));
					int article = stoi(input.at(2));
					readArticle(newsgroup, article); 
				} catch (exception& e){
					cout << "Wrong input, expected 'read <newsgroup-number> <article-number>', e.g. 'read 2 3'" << endl; 
				}
   			} else {
   				cout << "Wrong input, expected 'read <newsgroup-number> <article-number>', e.g. 'read 2 3'" << endl;
   			}
   		}
   		else if(first == "create"){
   			string newsgroup;
   			if(n==1){
   				cout << "Wrong input, name of newsgroup needs to be included" << endl; 
   			} else {
   				for(int i=1; i<n; i++){
   					newsgroup += (input.at(i) + " "); 
   				}
   				createNewsgroup(newsgroup); 
   			}
   		}
   		else if(first == "delete"){
   			if(n == 3){
   				try{
   					int newsgroup = stoi(input.at(1));
					int article = stoi(input.at(2));
					deleteArticle(newsgroup, article); 
   				} catch (exception& e){
					cout << "Wrong input, expected 'delete <newsgroup-number> <article-number>', e.g. 'delete 2 3'" << endl; 
				}
   			} 
   			else if(n == 2){
   				try{
   					int newsgroup = stoi(input.at(1));
					deleteNewsgroup(newsgroup); 
   				} catch (exception& e){
					cout << "Wrong input, expected 'delete <newsgroup-number>', e.g. 'delete 2'" << endl; 
				}
   			} else {
   				cout << "Wrong input, expected 'delete <newsgroup>' or 'delete <newsgroup> <article>'. Type help for more" << endl; 
   			}
   		}
   		else if(first == "write"){
   			writeArticle();
   		}
   		else if(first == "help"){
   			printInstructions();
   		}
   		else if(first == "exit"){
   			exit(1); 
   		}
   		else{
   			cout << "Wrong input. Type 'help' to see all options" << endl; 
   		}
	}

	return 0;
}