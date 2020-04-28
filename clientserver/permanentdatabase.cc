#include "permanentdatabase.h"
#include <utility>
#include <map>
#include <iostream>
#include "filesystemutils.h"
#include <cstdlib>
#include <fstream>  //for ofstream/ifstream
#include "dirent.h"
#include <stdio.h>
#include <string.h>


#if defined(_WIN32)
std::string dirSeparator = "\\";
#else
std::string dirSeparator = "/";
#endif

PermanentDatabase::PermanentDatabase() {
	if (makeDirectory("db")==-1) {
		std::cout << "Data detected, reading..." << std::endl;
		loadData();
	} else {
		std::cout << "No data detected. Initiating database." << std::endl; 
		std::ofstream outfile("db" + dirSeparator + "data");
		outfile << 0;
		outfile.close();
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

int PermanentDatabase::createNewsgroup(const std::string name) {
	if (newsgroupNames.count(name) > 0) {
		return NAME_TAKEN;
	}
	newsgroupNames.insert(name);

	Newsgroup ng;
	int id = getNewId();
	ng.id = id;
	ng.name = name;
	
	newsgroups[id]=ng;
	std::string temp = "db" + dirSeparator + std::to_string(id);
	if (makeDirectory(temp)==-1) {
		std::cout << "Warning: Could not create directory '" << id << "'." << std::endl;
		return false;
	}
	std::ofstream outfile("db" + dirSeparator + std::to_string(id) + dirSeparator + "data");
	outfile << ng.name << std::endl;
	outfile.close();

	return OK;
}

int PermanentDatabase::deleteNewsgroup(const int groupId) {
	if (newsgroupExists(groupId)) {
		deleteFile("db" + dirSeparator + std::to_string(groupId) + dirSeparator + "data");
		Newsgroup ng = newsgroups.at(groupId);
		newsgroupNames.erase(ng.name);
		for (auto a : ng.articles) {
			if (deleteFile("db" + dirSeparator + std::to_string(groupId) + dirSeparator + std::to_string(a.second.id)) != 0) {
				std::cout << "Warning: 'db" << dirSeparator << std::to_string(groupId) << dirSeparator << std::to_string(a.second.id);
				std::cout << "' could not be deleted." << std::endl;
			}
		}
		if (deleteDirectory("db" + dirSeparator + std::to_string(groupId)) != 0) {
			std::cout << "Warning: 'db" << dirSeparator << std::to_string(groupId) << "' could not be deleted." << std::endl;
		}
		newsgroups.erase(groupId);
		return OK;
	}
	return NO_NG;
}

std::pair<std::vector<Article>,int> PermanentDatabase::listArticles(const int groupId) const {
	std::vector<Article> result;
	if (newsgroupExists(groupId)) {
		for (auto thing : newsgroups.at(groupId).articles) {
			result.push_back(thing.second);
		}
		return std::make_pair(result,OK);
	}
	return std::make_pair(result, NO_NG);
}

std::pair<Article,int> PermanentDatabase::readArticle(const int groupId, const int articleId) const {
	if (newsgroupExists(groupId)) {
		if (articleExists(groupId, articleId)) {
			return std::make_pair(newsgroups.at(groupId).articles.at(articleId),OK);
		} else {
			return std::make_pair(Article(), NO_ART);
		}
	}
	return std::make_pair(Article(), NO_NG);
}

int PermanentDatabase::writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) {
	if (!newsgroupExists(groupId)) {
		return NO_NG;
	}
	
	int id = getNewId();
	Article article;
	article.author = author;
	article.id = id;
	article.text = text;
	article.title = title;

	newsgroups.at(groupId).articles[id] = article;

	std::ofstream outfile("db" + dirSeparator + std::to_string(groupId) + dirSeparator + std::to_string(article.id));
	outfile << article.title << std::endl;
	outfile << article.author << std::endl;
	outfile << article.text << std::endl;
	outfile.close();

	return OK;
}

int PermanentDatabase::deleteArticle(const int groupId, const int articleId) {
	if (newsgroupExists(groupId)) {
		std::string address = "db" + dirSeparator + std::to_string(groupId) + dirSeparator + std::to_string(articleId);
		if (articleExists(groupId, articleId)) {
			newsgroups.at(groupId).articles.erase(articleId);
			if (deleteFile(address) != 0) {
				std::cout << "Warning: In 'deleteArticle', could not delete file '" << address << "'" << std::endl;
			}
		} else {
			return NO_ART;
		}


		return OK;
	}
	return NO_NG;
}

bool PermanentDatabase::loadData() {

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir("db")) != NULL) {
		bool result = false;
		while ((ent = readdir(dir)) != NULL) {
			std::string directoryName = ent->d_name;
			if (directoryName!="."&&directoryName!=".."&&directoryName!="data") {
				Newsgroup ng;
				
				std::ifstream infile("db" + dirSeparator + directoryName + dirSeparator + "data");
				std::string line;
				std::getline(infile, line);

				newsgroupNames.insert(line);
				ng.name = line;

				infile.close();
				
				ng.id = std::stoi(directoryName);
				newsgroups[ng.id] = ng;
				
				result=readFiles(directoryName);

			}
			
		}
		closedir(dir);
		return result;
	}
	else {
		/* could not open directory */
		std::cout << "Warning: In 'loadData', could not open database" << std::endl;
		return false;
	}

}

bool PermanentDatabase::readFiles(const std::string directory) {
	int ngId = std::stoi(directory);
	DIR* dir;
	struct dirent* ent;
	std::string address = "db" + dirSeparator + directory;
	if ((dir = opendir(address.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string filename = ent->d_name;
			if (filename != "." && filename != ".." && filename != "data") {
				std::ifstream infile("db" + dirSeparator + std::to_string(std::stoi(directory))+dirSeparator+filename);
				Article art;
				art.id = std::stoi(filename);
				std::string line;
				std::getline(infile, line);
				art.title = line;
				std::getline(infile, line);
				art.author = line;
				std::getline(infile, line);
				art.text = line;
					
				newsgroups.at(ngId).articles[art.id] = art;
				infile.close();
			}

		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		std::cout << "Warning: In 'readFiles', could not open newsgroup directory " << std::to_string(ngId) << std::endl;
		return false;
	}
	return false;
}

int PermanentDatabase::getNewId() {
	std::ifstream infile("db" + dirSeparator + "data");
	std::string line;
	std::getline(infile, line);

	int id = std::stoi(line);

	infile.close();

	std::ofstream outfile("db" + dirSeparator + "data", std::ofstream::trunc);
	outfile << id + 1;
	outfile.close();

	return id;
}

bool PermanentDatabase::newsgroupExists(const int ng) const {
	return (newsgroups.count(ng) > 0);
}
bool PermanentDatabase::articleExists(const int ng, const int art) const {
	return (newsgroups.at(ng).articles.count(art) > 0);
}

/*int main() {
	Database* db = new PermanentDatabase();
	if (db->createNewsgroup("Fabbe news")) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Created newsgroup. ";
	int grpId = db->listNewsgroups().at(0).id;
	std::cout << "Group id: " << grpId << std::endl;
	
	
	if (db->writeArticle(grpId, "Morning News", "Fabbe", "Today nothing happened. Not very interesting, was it?")) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Wrote article. ";
	int artId = db->listArticles(grpId).at(0).id;
	std::cout << "Article id: " << artId << std::endl;
	std::cout << "	Article text: " << db->readArticle(grpId, artId).text << std::endl;
	
	if (db->deleteArticle(grpId, artId)) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Deleted article. Number of articles is now: " << db->listArticles(grpId).size() << std::endl;
	
	db->writeArticle(grpId, "Title 1", "Fabbe", "Text1");
	db->writeArticle(grpId, "Title 2", "Fabbe", "Text2");
	db->writeArticle(grpId, "Title 3", "Fabbe", "Text3");
	db->writeArticle(grpId, "Title 4", "Fabbe", "Text4");
	db->writeArticle(grpId, "Title 5", "Fabbe", "Text5");

	PermanentDatabase db2 = PermanentDatabase();
	
	if (db2.readArticle(0,6).text == "Text5") {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Article id, group id and text is the same after database reload." << std::endl;
	


	
	if (db2.deleteNewsgroup(grpId)) {
		std::cout << "Success! ";
	} else {
		std::cout << "Failure! ";
	}
	std::cout << "Deleted newsgroup. Number of news groups is now: " << db2.listNewsgroups().size() << std::endl;
	
	
}*/