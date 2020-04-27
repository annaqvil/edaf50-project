//• List newsgroups.
//• Createand delete newsgroups.
//• List articles in a newsgroup.
//• Read, write and delete articles in a newsgroup.

#pragma once

#include <vector>
#include <string>
#include <map>
#include <utility>

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
    static const int OK = 1;
    static const int NO_NG = -2;
    static const int NO_ART = -3;
    static const int NAME_TAKEN = -4;


    Database() {}
    virtual ~Database() = default;
    virtual std::vector<Newsgroup> listNewsgroups() const = 0;  // "= 0" part makes this method pure virtual, and
                                                                // also makes this class abstract.
    virtual int createNewsgroup(const std::string name) = 0;
    virtual int deleteNewsgroup(const int articleId) = 0;
    virtual std::pair<std::vector<Article>,int> listArticles(const int groupId) const = 0;
    virtual std::pair <Article, int> readArticle(const int groupId, const int articleId) const = 0;
    virtual int writeArticle(const int groupId, const std::string title, const std::string author, const std::string text) = 0;
    virtual int deleteArticle(const int groupId, const int articleId) = 0;

protected:
    virtual bool newsgroupExists(const int ng) const = 0;
    virtual bool articleExists(const int ng, const int art) const = 0;
    int idCounter = 0;
};
