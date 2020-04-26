#pragma once

#include "database.h"
#include <vector>
#include <string>
#include <map>
#include <set>

class PermanentDatabase : Database
{
public:
    PermanentDatabase();
    //~PermanentDatabase() override;
    std::vector<Newsgroup> listNewsgroups() const override;
    bool createNewsgroup(const std::string name) override;
    bool deleteNewsgroup(const int articleId) override;
    std::vector<Article> listArticles(const int groupId) const override;
    Article readArticle(const int groupId, const int articleId) const override;
    bool writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) override;
    bool deleteArticle(const int groupId, const int articleId) override;
private:
    std::map<int, Newsgroup> newsgroups;
    std::set<std::string> newsgroupNames;
};