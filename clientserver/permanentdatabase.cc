#include "permanentdatabase.h"
#include <utility>
#include <map>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <direct.h>
#include <fstream>  //for outfile


PermanentDatabase::PermanentDatabase() {
	if (_mkdir("db")==-1) {
		std::cout << "not nice" << std::endl;
	} else {
		std::cout << "nice" << std::endl;
	}
}
//VolatileDatabase::~VolatileDatabase() {}

std::vector<Newsgroup> PermanentDatabase::listNewsgroups() const {
	std::vector<Newsgroup> result;
	for (auto pair : newsgroups) {
		result.push_back(pair.second);
	}
	return result;
}    
bool PermanentDatabase::createNewsgroup(const std::string name) {
	Newsgroup ng;
	int id = idCounter++;
	ng.id = id;
	ng.name = name;
	if (newsgroupNames.count(name)>0) {
		return false;
	}
	newsgroups[id]=ng;
	std::string temp = "db\\" + std::to_string(id);
	if (_mkdir(temp.c_str())==-1) {
		std::cout << "Warning: Could not create directory '" << id << "'." << std::endl;
		return false;
	}
	std::ofstream outfile("db\\" + std::to_string(id) + "\\data.txt");
	outfile << ng.name << std::endl;

	return true;
};

bool PermanentDatabase::deleteNewsgroup(const int groupId) {
	if (newsgroups.erase(groupId) > 0) {
		
		return true;
	}
	return false;
}
std::vector<Article> PermanentDatabase::listArticles(const int groupId) const {
	std::vector<Article> result;
	for (auto thing : newsgroups.at(groupId).articles) {
		result.push_back(thing.second);
	}
	return result;
}

Article PermanentDatabase::readArticle(const int groupId, const int articleId) const {
	return newsgroups.at(groupId).articles.at(articleId);
}
bool PermanentDatabase::writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) {
	int id = idCounter++;	
	Article article;
	article.author = author;
	article.id = id;
	article.text = text;
	article.title = title;

	if (newsgroups.count(groupId) > 0) {
		newsgroups.at(groupId).articles[id] = article;
		return true;
	}
	else {
		std::cout << "Warning: News group not found." << std::endl;
	}
	return false;
}
bool PermanentDatabase::deleteArticle(const int groupId, const int articleId) {
	if (newsgroups.count(groupId > 0)) {
		return newsgroups.at(groupId).articles.erase(articleId);
	} else {
		std::cout << "Warning: News group not found." << std::endl;
	}
	return false;
}

int main() {
	PermanentDatabase db;
	if (db.createNewsgroup("Fabbe news")) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Created newsgroup. ";
	int grpId = db.listNewsgroups().at(0).id;
	std::cout << "Group id: " << grpId << std::endl;
	
	if (db.writeArticle(grpId, "Morning News", "Fabbe", "Today nothing happened. Not very interesting, was it?")) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Wrote article. ";
	int artId = db.listArticles(grpId).at(0).id;
	std::cout << "Article id: " << artId << std::endl;
	std::cout << "	Article text: " << db.readArticle(grpId, artId).text << std::endl;
	
	if (db.deleteArticle(grpId, artId)) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Deleted article. Number of articles is now: " << db.listArticles(grpId).size() << std::endl;
	
	db.writeArticle(grpId, "Title 1", "Fabbe", "Text1");
	db.writeArticle(grpId, "Title 2", "Fabbe", "Text2");
	db.writeArticle(grpId, "Title 3", "Fabbe", "Text3");
	db.writeArticle(grpId, "Title 4", "Fabbe", "Text4");
	db.writeArticle(grpId, "Title 5", "Fabbe", "Text5");

	if (db.deleteNewsgroup(grpId)) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Deleted newsgroup. Number of news groups is now: " << db.listNewsgroups().size() << std::endl;
	
}