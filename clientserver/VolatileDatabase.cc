#include "volatileDatabase.h"
#include <utility>
#include <map>
#include <iostream>

//VolatileDatabase::VolatileDatabase() {}
VolatileDatabase::~VolatileDatabase() {}

std::vector<Newsgroup> VolatileDatabase::listNewsgroups() const {
	std::vector<Newsgroup> result;
	for (auto pair : newsgroups) {
		result.push_back(pair.second);
	}
	return result;
}    
bool VolatileDatabase::createNewsgroup(const std::string name) {
	Newsgroup ng;
	//TODO set parameters
	newsgroups[5]=ng; //TODO id
	return true;
};

bool VolatileDatabase::deleteNewsgroup(const int articleId) {
	return newsgroups.erase(articleId);
}
std::vector<Article> VolatileDatabase::listArticles(const int groupId) const {
	std::vector<Article> result;
	for (auto thing : newsgroups.at(groupId).articles) {
		result.push_back(thing.second);
	}
	return result;
}

Article VolatileDatabase::readArticle(const int groupId, const int articleId) const {
	return newsgroups.at(groupId).articles.at(articleId);
}
bool VolatileDatabase::writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) {
	int id = 5; //TODO id
	Article article;
	article.author = author;
	article.id = id;
	article.text = text;
	article.title = title;

	if (newsgroups.count(groupId) > 0) {
		newsgroups.at(groupId).articles[5] = article;
		return true;
	}
	else {
		std::cout << "Warning: News group not found." << std::endl;
	}
	return false;
}
bool VolatileDatabase::deleteArticle(const int groupId, const int articleId) {
	if (newsgroups.count(groupId > 0)) {
		return newsgroups.at(groupId).articles.erase(articleId);
	} else {
		std::cout << "Warning: News group not found." << std::endl;
	}
	return false;
}
