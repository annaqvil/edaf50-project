#pragma once

#include "database.h"
#include <vector>
#include <string>
#include <map>
#include <set>

class VolatileDatabase : public Database
{
public:
    VolatileDatabase() {}
    //~VolatileDatabase() override;
    std::vector<Newsgroup> listNewsgroups() const override;
    int createNewsgroup(const std::string name) override;
    int deleteNewsgroup(const int articleId) override;
    std::pair<std::vector<Article>,int> listArticles(const int groupId) const override;
    std::pair<Article,int> readArticle(const int groupId, const int articleId) const override;
    int writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) override;
    int deleteArticle(const int groupId, const int articleId) override;
protected:
    bool newsgroupExists(const int ng) const override;
    bool articleExists(const int ng, const int art) const override;
private:
    std::map<int, Newsgroup> newsgroups;
    std::set<std::string> newsgroupNames;
};
