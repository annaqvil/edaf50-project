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
#include "database.h"
#include "permanentdatabase.h"
#include "volatiledatabase.h"
#include <vector>
#include <string.h>

using std::cerr;
using std::endl;
using std::stoi;
using std::cout;
using std::string;

Server init(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: ourserver port-number volatile|permanent" << endl;
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

void dbError(int code, MessageHandler& ms) {
	ms.sendCode(Protocol::ANS_NAK);
	if (code == Database::NAME_TAKEN) {
		ms.sendCode(Protocol::ERR_NG_ALREADY_EXISTS);
	}
	else if (code == Database::NO_ART) {
		ms.sendCode(Protocol::ERR_ART_DOES_NOT_EXIST);
	}
	else if (code == Database::NO_NG) {
		ms.sendCode(Protocol::ERR_NG_DOES_NOT_EXIST);
	}
	else {
		cerr << "Unknown database error code." << std::endl;
	}
	ms.sendCode(Protocol::ANS_END);
}

void listNewsgroups(Database* db, MessageHandler& ms) {
	if (ms.recvCommand() != Protocol::COM_END)
	{
		cerr << "Protocol Error. " << endl;
		return;
	}
	ms.sendCode(Protocol::ANS_LIST_NG);
	std::vector<Newsgroup> groups = db->listNewsgroups();
	ms.sendIntParameter(groups.size());
	for (Newsgroup n : groups) {
		ms.sendIntParameter(n.id);
		ms.sendStringParameter(n.name);
	}
	ms.sendCode(Protocol::ANS_END);
	return;
}


void listArticles(Database* db, MessageHandler& ms) {
	int groupId =	ms.recvIntParameter();
	
	if (ms.recvCommand() != Protocol::COM_END)
	{
		cerr << "Protocol Error. " << endl;
		return;
	}

	ms.sendCode(Protocol::ANS_LIST_ART);
	std::pair<std::vector<Article>, int > entry = db->listArticles(groupId);
	if (entry.second != db->OK) {
		dbError(entry.second, ms);
		return;
	}

	ms.sendCode(Protocol::ANS_ACK);
	std::vector<Article> articles = entry.first;
	ms.sendIntParameter(articles.size());
	for (Article a : articles) {
		ms.sendIntParameter(a.id);
		ms.sendStringParameter(a.title);
	}
	ms.sendCode(Protocol::ANS_END);
}

void readArticle(Database* db, MessageHandler& ms) {
	int groupId = ms.recvIntParameter();
	int articleId = ms.recvIntParameter(); //TODO maybe try/catch
	if (Protocol::COM_END == ms.recvCommand()) {
		std::pair<Article,int> entry = db->readArticle(groupId, articleId);
		ms.sendCode(Protocol::ANS_GET_ART);
		if (entry.second != Database::OK) {
			dbError(entry.second, ms);
			return;
		}
		Article a = entry.first;
		ms.sendStringParameter(a.title);
		ms.sendStringParameter(a.author);
		ms.sendStringParameter(a.text);
		ms.sendCode(Protocol::ANS_END);
	} else {
		cerr << "Protocol Error." << endl;
	}
}

void createNewsgroup(Database* db, MessageHandler& ms) {
	std::string name = ms.recvStringParameter();
	if (Protocol::COM_END == ms.recvCommand()) {
		int code = db->createNewsgroup(name);
		if (code!=Database::OK) {
			dbError(code, ms);
			return;
		}
		ms.sendCode(Protocol::ANS_CREATE_NG);
		ms.sendCode(Protocol::ANS_ACK);
		ms.sendCode(Protocol::ANS_END);
	}
}

void writeArticle(Database* db, MessageHandler& ms) {
	int group_nr;
	string title;
	string author;
	string text;
	group_nr = ms.recvIntParameter();
	title = ms.recvStringParameter();
	author = ms.recvStringParameter();
	text = ms.recvStringParameter();

	if (ms.recvCommand() != Protocol::COM_END) {
		cerr << "Protocol error" << endl;
		return;
	}
	
	int code = db->writeArticle(group_nr, title, author, text);
	if (code != Database::OK) {
		dbError(code, ms);
	}

	ms.sendCode(Protocol::ANS_ACK);
	ms.sendCode(Protocol::ANS_END);
}


void deleteNewsgroup(Database* db, MessageHandler& ms) {
	//COM_DELETE_NG num_p COM_END
	//ANS_DELETE_NG [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
	int newsgroup = ms.recvIntParameter();
	if (ms.recvCommand() == Protocol::COM_END) {
		int res = db->deleteNewsgroup(newsgroup);
		if (res == Database::OK) {
			ms.sendCode(Protocol::ANS_DELETE_NG);
			ms.sendCode(Protocol::ANS_ACK);
			ms.sendCode(Protocol::ANS_END);
		}
		else {
			dbError(res, ms);
		}
	}
	else {
		cerr << "Protocol error" << endl;
	}
}

void deleteArticle(Database* db, MessageHandler& ms) {
	//COM_DELETE_ART num_p num_p COM_END
	//ANS_DELETE_ART [ANS_ACK |
	//ANS_NAK [ERR_NG_DOES_NOT_EXIST | ERR_ART_DOES_NOT_EXIST]] ANS_END
	int newsgroup = ms.recvIntParameter();
	int article = ms.recvIntParameter();
	if(ms.recvCommand() == Protocol::COM_END){
		int res = db->deleteArticle(newsgroup, article);
		if(res == Database::OK){
			ms.sendCode(Protocol::ANS_DELETE_ART);
			ms.sendCode(Protocol::ANS_ACK);
			ms.sendCode(Protocol::ANS_END);
		} else{
			dbError(res, ms);
		}
	}else{
		cerr << "Protocol error" << endl; 
	}
}


int main(int argc, char* argv[])
{
	auto server = init(argc, argv);
	Database* db;
	if(!strcmp(argv[2], "permanent")){
		db = new PermanentDatabase();
	} 
	else if(!strcmp(argv[2], "volatile")){
		db = new VolatileDatabase();
	} else{
		cout << "Wrong input, please write 'permanent' or 'volatile'" << endl;
		exit(2);
	}

	while (true) {
		auto conn = server.waitForActivity();
		MessageHandler ms(conn);
		if (conn != nullptr) {
			//try {
			Protocol p = ms.recvCommand();
			switch (p) {
			case Protocol::COM_LIST_NG:
				listNewsgroups(db,ms);
				break;
			case Protocol::COM_LIST_ART:
				listArticles(db,ms); 
				break;
			case Protocol::COM_GET_ART:
				readArticle(db, ms); 
				break; 
			case Protocol::COM_CREATE_NG:
				createNewsgroup(db, ms);
				break;
			case Protocol::COM_DELETE_ART:
				deleteArticle(db, ms);
				break;
			case Protocol::COM_DELETE_NG:
				deleteNewsgroup(db, ms);
				break;
			case Protocol::COM_CREATE_ART:
				writeArticle(db, ms); 
				break;
			default:
				cerr << "Protocol Error" << endl; 
			}
		}
		else {
			conn = std::make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
	return 0;

	delete db;
}