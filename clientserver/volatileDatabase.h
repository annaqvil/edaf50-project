#pragma once

#include "database.h"
#include <vector>
#include <string>
#include <map>

class VolatileDatabase : Database
{
public:
    VolatileDatabase() {}
    ~VolatileDatabase() {}
    std::vector<Newsgroup> listNewsgroups() const override;    // "= 0" part makes this method pure virtual, and
                                                            // also makes this class abstract.
    bool createNewsgroup(const std::string name) const override;
    bool deleteNewsgroup(const int articleId) const override;
    std::vector<Article> listArticles(const int groupId) const override;
    Article readArticle(const int articleId) const override;
    bool writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) const override;
    bool deleteArticle(const int groupId) const override;
private:
    std::map<int, Newsgroup> newsgroups;
};