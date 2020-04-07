#include "DatabaseAccess.h"

DatabaseAccess::DatabaseAccess()
{
    con = mysql_init(NULL); //初始化链接变量
    if (con == NULL)
    {
        cerr << "init_error: " << mysql_error(con) << endl;
        exit(1);
    }
}

DatabaseAccess::~DatabaseAccess()
{
    if (con)
        mysql_close(con);
}

bool DatabaseAccess::Init_DB(string host, string user, string pwd, string db_name, string charset)
{
    //用mysql_real_connect建立一个连接
    con = mysql_real_connect(con, host.c_str(), user.c_str(), pwd.c_str(),
                             db_name.c_str(), 3306, NULL, 0);
    if (con == NULL)
    {
        cerr << "connect_error: " << mysql_errno(con) << endl;
        exit(1);
    }
    string sql = "set names " + charset;
    mysql_query(con, sql.c_str());
    return true;
}

bool DatabaseAccess::exec(string sql)
{
    //执行sql查询语句,成功返回0，失败返回非0
    if (mysql_query(con, sql.c_str()))
    {
        cerr << "query error: " << mysql_error(con) << endl;
        exit(1);
    }
    else
    {
        //获取查询的结果
        res = mysql_store_result(con);
        if (res)
        {
            //返回查询的行数
            for (int i = 0; i < mysql_num_rows(res); ++i)
            {
                row = mysql_fetch_row(res);
                if (row < 0)
                    break;

                //结果集中的字段数
                for (int j = 0; j < mysql_num_fields(res); ++j)
                    cout << row[j] << " ";
                cout << endl;
            }
        }
        else //res==NULL
        {
            if (mysql_field_count(con) == 0) //返回insert update delete 等影响的列数
                int num_rows = mysql_affected_rows(con);
            else
            {
                cout << "get result error: " << mysql_error(con) << endl;
                return false;
            }
        }
    }
    //释放结果集
    mysql_free_result(res);
    return true;
}

int DatabaseAccess::Login(string auth_str)
{
    if (auth_str[7] == '*') //base
    {
        string sql = "select count(*) from student where stu_password=\"" + auth_str.substr(8, 32) + "\" and stu_no=\"" + auth_str.substr(0, 7) + "\"";
        if (mysql_query(con, sql.c_str()))
        {
            cerr << "query error: " << mysql_error(con) << endl;
            exit(1);
        }
        else
        {
            res = mysql_store_result(con);
            if (res)
            {
                row = mysql_fetch_row(res);
                if (atoi(row[0]) == 1)
                    return 0; // login success
            }
            else
                return -1; // login failed
        }
    }
    else if (auth_str[7] == '-') //competition
    {
        string stuno = auth_str.substr(0, 7);
        reverse(stuno.begin(), stuno.end());
        string sql = "select count(*) from student where stu_password=\"" + auth_str.substr(8, 32) + "\" and stu_no=\"" + stuno + "\"";
        if (mysql_query(con, sql.c_str()))
        {
            cerr << "query error: " << mysql_error(con) << endl;
            exit(1);
        }
        else
        {
            res = mysql_store_result(con);
            if (res)
            {
                row = mysql_fetch_row(res);
                if (atoi(row[0]) == 1)
                    return 1; // login success
            }
            else
                return -1; // login failed
        }
    }
    return -1;
}

int DatabaseAccess::GameOver_Write(map<string, int> gameinfo, map<string, string> block)
{
    // {"Type", "Content", "GameID","Step","Score","MaxValue"}
    string sql = "insert into `game` (stu_no, game_mode, game_mapid, game_row, game_col, game_time, game_score)";
    sql += " values(\"" + to_string(gameinfo["StuNo"]) + "\",\"" + to_string(gameinfo["GameType"]) + "\",\"" + block["GameID"];
    sql += "\"," + to_string(gameinfo["Row"]) + "," + to_string(gameinfo["Col"]) + ",\"";
    sql += Tools::Get_Time() + "\"," + block["Score"] + ")";
    cout << sql << endl;
    if (mysql_query(con, sql.c_str()))
    {
        cerr << "query error: " << mysql_error(con) << endl;
        exit(1);
    }
    return 0;
}