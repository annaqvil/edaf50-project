#include "volatiledatabase.h"
#include <utility>
#include <map>
#include <iostream>


//VolatileDatabase::VolatileDatabase() {}
//VolatileDatabase::~VolatileDatabase() {}

std::vector<Newsgroup> VolatileDatabase::listNewsgroups() const {
	std::vector<Newsgroup> result;
	for (auto pair : newsgroups) {
		result.push_back(pair.second);
	}
	return result;
}    
int VolatileDatabase::createNewsgroup(const std::string name) {
	if (newsgroupNames.count(name)>0) {
		return NAME_TAKEN;
	}
	Newsgroup ng;
	int id = idCounter++;
	ng.id = id;
	ng.name = name;
	newsgroups[id]=ng;
	return OK;
}

int VolatileDatabase::deleteNewsgroup(const int groupId) {
	if (newsgroupExists(groupId)) {
		newsgroups.erase(groupId);
		return OK;
	}
	return NO_NG;
}

std::pair<std::vector<Article>,int> VolatileDatabase::listArticles(const int groupId) const {
	std::vector<Article> result;
	if (newsgroupExists(groupId)) {
		for (auto thing : newsgroups.at(groupId).articles) {
			result.push_back(thing.second);
		}
		return std::make_pair(result,OK);
	}
	return std::make_pair(result, NO_NG);
}

std::pair<Article,int> VolatileDatabase::readArticle(const int groupId, const int articleId) const {
	if (newsgroupExists(groupId)) {
		if (articleExists(groupId,articleId)) {
			return std::make_pair(newsgroups.at(groupId).articles.at(articleId),OK);
		} else {
			return std::make_pair(Article(), NO_ART);
		}
	}
	return std::make_pair(Article(), NO_NG);
}

int VolatileDatabase::writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) {
	if (!newsgroupExists(groupId)) {
		return NO_NG;
	}
	
	int id = idCounter++;	
	Article article;
	article.author = author;
	article.id = id;
	article.text = text;
	article.title = title;

	newsgroups.at(groupId).articles[id] = article;
	return OK;
}
int VolatileDatabase::deleteArticle(const int groupId, const int articleId) {
	if (newsgroupExists(groupId)) {
		if (articleExists(groupId,articleId)) {
			newsgroups.at(groupId).articles.erase(articleId);
			return OK;
		} else {
			return NO_ART;
		}
		
	}
	return NO_NG;
}

bool VolatileDatabase::newsgroupExists(const int ng) const {
	return (newsgroups.count(ng) > 0);
}
bool VolatileDatabase::articleExists(const int ng, const int art) const {
	return (newsgroups.at(ng).articles.count(art) > 0);
}

/*int main() {
	VolatileDatabase db;
	if (db.createNewsgroup("´Fabbe news")) {
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
	*/
	

