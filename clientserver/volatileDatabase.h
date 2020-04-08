#pragma once

#include "database.h"
#include <vector>
#include <string>
#include <map>

class VolatileDatabase : Database
{
public:
    VolatileDatabase() {}
    ~VolatileDatabase() override;
    std::vector<Newsgroup> listNewsgroups() const override;
    bool createNewsgroup(const std::string name) override;
    bool deleteNewsgroup(const int articleId) override;
    std::vector<Article> listArticles(const int groupId) const override;
    Article readArticle(const int groupId, const int articleId) const override;
    bool writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) override;
    bool deleteArticle(const int groupId, const int articleId) override;
private:
    std::map<int, Newsgroup> newsgroups;
};