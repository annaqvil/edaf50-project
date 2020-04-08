//• List newsgroups.
//• Createand delete newsgroups.
//• List articles in a newsgroup.
//• Read, write and delete articles in a newsgroup.

#pragma once

#include <vector>
#include <string>
#include <map>

struct Article {
    int id;
    std::string title;
    std::string author;
    std::string text;
};

struct Newsgroup {
    int id;
    std::string name;
    std::map<int, Article> articles;
};

class Database
{
public:
    Database() {}
    virtual ~Database() = 0;
    virtual std::vector<Newsgroup> listNewsgroups() const = 0;  // "= 0" part makes this method pure virtual, and
                                                                // also makes this class abstract.
    virtual bool createNewsgroup(const std::string name) = 0;
    virtual bool deleteNewsgroup(const int articleId) = 0;
    virtual std::vector<Article> listArticles(const int groupId) const = 0;
    virtual Article readArticle(const int groupId, const int articleId) const = 0;
    virtual bool writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) = 0;
    virtual bool deleteArticle(const int groupId, const int articleId) = 0;
private:
    int idCounter = 0;
};
