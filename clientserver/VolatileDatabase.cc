#include "volatileDatabase.h"

VolatileDatabase::VolatileDatabase() {}
VolatileDatabase::~VolatileDatabase() {}
std::vector<Newsgroup> VolatileDatabase::listNewsgroups() const {

}    
bool VolatileDatabase::createNewsgroup(const std::string name) const {

};
bool VolatileDatabase::deleteNewsgroup(const int articleId) const {

}
std::vector<Article> VolatileDatabase::listArticles(const int groupId) const {

}
Article VolatileDatabase::readArticle(const int articleId) const {

}
bool VolatileDatabase::writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) const {

}
bool VolatileDatabase::deleteArticle(const int groupId) const {

}
