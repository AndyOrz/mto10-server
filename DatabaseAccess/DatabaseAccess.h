#ifndef DatabaseAccess_H
#define DatabaseAccess_H

#include "../common.h"
#include <mysql/mysql.h>
#include "../Tools/Tools.h"

class DatabaseAccess
{
private:
    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW res_row;
    bool exec(string sql);
public:
    DatabaseAccess();
    ~DatabaseAccess();
    bool Init_DB(string host, string user, string pwd, string db_name, string charset);
    int Login(string auth_str);
    int GameOver_Write(map<string, int> &gameinfo, map<string, string> &block);
    string SelectStest(int mapid, int row, int col, int limit);
};

#endif