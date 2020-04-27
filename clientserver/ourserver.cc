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
		if (entry.second != Database::OK) {
			dbError(entry.second, ms);
			return;
		}
		Article a = entry.first;
		ms.sendCode(Protocol::ANS_GET_ART);
		ms.sendStringParameter(a.title);
		ms.sendStringParameter(a.author);
		ms.sendStringParameter(a.text);
		ms.sendCode(Protocol::ANS_END);
	} else {
		cerr << "Protocol Error." << endl;
	}
}
/*
void createNewsgroup(Database* db, MessageHandler& ms) {
	std::string name = ms.recvStringParameter();

	//LALLAA
	ms.sendCode(Protocol::COM_END);

	if (ms.recvCommand() == Protocol::ANS_CREATE_NG) {
		if (handleAck(ms)) {
			cout << "Succesfully created newsgroup" << endl;
		}
		handleEnd(ms);
	}
	else {
		cerr << "Protocol Error. " << endl;
	}
}

void writeArticle(Database* db, MessageHandler& ms) {
	//COM_CREATE_ART group_nr title author text COM_END
	//ANS_CREATE_ART [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
	int group_nr;
	string title;
	string author;
	string text;
	cout << "Provide the following information: " << endl;
	cout << "The newsgroup-number of the newsgroup you want to add the article to: ";
	while (!(cin >> group_nr)) {
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cout << "Invalid input, you must type a number.  Try again: ";
	}
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	cout << "Title: ";
	getline(cin, title);
	cout << "Author: ";
	getline(cin, author);
	cout << "Content (type :q on a single line to stop writing):" << endl;
	string current;
	while (current != ":q") {
		getline(cin, current);
		if (current != ":q")
			text += current + "\n";
	}
	ms.sendCode(Protocol::COM_CREATE_ART);
	ms.sendIntParameter(group_nr);
	ms.sendStringParameter(title);
	ms.sendStringParameter(author);
	ms.sendStringParameter(text);
	ms.sendCode(Protocol::COM_END);

	if (ms.recvCommand() == Protocol::ANS_CREATE_ART) {
		if (handleAck(ms))
			cout << "Article was succesfully created" << endl;
		handleEnd(ms);
	}
	else {
		cerr << "Protocol Error. " << endl;
	}

}


void deleteNewsgroup(Database* db, MessageHandler& ms) {
	//COM_DELETE_NG num_p COM_END
	//ANS_DELETE_NG [ANS_ACK | ANS_NAK ERR_NG_DOES_NOT_EXIST] ANS_END
	ms.sendCode(Protocol::COM_DELETE_NG);
	ms.sendIntParameter(newsgroup);
	ms.sendCode(Protocol::COM_END);

	if (ms.recvCommand() == Protocol::ANS_DELETE_NG) {
		if (handleAck(ms))
			cout << "Newsgroup succesfully deleted" << endl;
	}
	else {
		cerr << "Protocol Error. " << endl;
	}
}

void deleteArticle(Database* db, MessageHandler& ms) {
	//COM_DELETE_ART num_p num_p COM_END
	//ANS_DELETE_ART [ANS_ACK |
	//ANS_NAK [ERR_NG_DOES_NOT_EXIST | ERR_ART_DOES_NOT_EXIST]] ANS_END
	ms.sendCode(Protocol::COM_DELETE_ART);
	ms.sendIntParameter(newsgroup);
	ms.sendIntParameter(article);
	ms.sendCode(Protocol::COM_END);

	if (ms.recvCommand() == Protocol::ANS_DELETE_ART) {
		if (handleAck(ms))
			cout << "Article succesfully deleted" << endl;
	}
	else {
		cerr << "Protocol Error. " << endl;
	}
}
*/

int main(int argc, char* argv[])
{
	auto server = init(argc, argv);
	Database* db = new PermanentDatabase(); // <-- titta ibland använder jag new

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
			case Protocol::COM_CREATE_NG:
				//createNewsgroup(db, ms);
				break;
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