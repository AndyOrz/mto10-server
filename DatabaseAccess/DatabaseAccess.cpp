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
                             db_name.c_str(), 0, NULL, 0);
    if (con == NULL)
    {
        cerr << "connect_error: " << mysql_errno(con) << endl;
        return false;
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
        return false;
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
                res_row = mysql_fetch_row(res);
                if (res_row < 0)
                    break;

                //结果集中的字段数
                for (int j = 0; j < mysql_num_fields(res); ++j)
                    cout << res_row[j] << " ";
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
            return -1;
        }
        else
        {
            res = mysql_store_result(con);
            if (res)
            {
                res_row = mysql_fetch_row(res);
                if (atoi(res_row[0]) == 1)
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
            return -1;
        }
        else
        {
            res = mysql_store_result(con);
            if (res)
            {
                res_row = mysql_fetch_row(res);
                if (atoi(res_row[0]) == 1)
                    return 1; // login success
            }
            else
                return -1; // login failed
        }
    }
    return -1;
}

int DatabaseAccess::GameOver_Write(map<string, int> &gameinfo, map<string, string> &block)
{
    // {"Type", "Content", "GameID","Step","Score","MaxValue"}
    timeval now_time;
	gettimeofday(&now_time,nullptr);
    int time_consumed=(now_time.tv_sec-gameinfo["GameStartTime_sec"])*1000000+(now_time.tv_usec-gameinfo["GameStartTime_usec"]);
    double weighted_score=(double)stoi(block["Score"])/gameinfo["Row"]/gameinfo["Col"];

    string sql = "insert into `game` (stu_no, game_mode, game_mapid, game_row, game_col, game_time, game_score,";
    sql += " game_step, game_maxvalue, game_time_consumed, game_result, game_weighted_score)";
    sql += " values(\"" + to_string(gameinfo["StuNo"]) + "\",\"" + to_string(gameinfo["GameType"]) + "\",\"" + block["GameID"];
    sql += "\"," + to_string(gameinfo["Row"]) + "," + to_string(gameinfo["Col"]) + ",\"";
    sql += Tools::Get_Time() + "\"," + block["Score"] ;
    sql += ","+block["Step"]+","+block["MaxValue"]+","+to_string(time_consumed)+",\""+block["Content"]+"\","+to_string(weighted_score)+ ")";
    //  cerr << Tools::Get_Time() << "Excute SQL:" << sql << endl;
    if (mysql_query(con, sql.c_str()))
    {
        cerr << "query error: " << mysql_error(con) << endl;
        return -1;
    }
    return 0;
}

string DatabaseAccess::SelectStest(int mapid, int row, int col, int limit)
{
    string sql="select game_time, game.stu_no, stu_name, game_mapid, game_row, game_col, ";
    sql+="game_score, game_step, game_maxvalue, game_time_consumed, game_result, game_weighted_score ";
    sql+="from game, student where game.stu_no=student.stu_no ";
    if (mapid!=-1)
        sql += "and game_mapid="+to_string(mapid)+" ";
    if (row!=-1)
        sql += "and game_row="+to_string(row)+" ";
    if (col!=-1)
        sql += "and game_col="+to_string(col)+" ";
    sql += "order by game_id desc ";
    if (limit!=-1)
        sql += "limit "+to_string(limit)+" ";
    else 
        sql += "limit "+to_string(10)+" ";


    string result="";
    //执行sql查询语句,成功返回0，失败返回非0
    if (mysql_query(con, sql.c_str()))
    {
        cerr << "query error: " << mysql_error(con) << endl;
        return "";
    }
    else
    {
        //获取查询的结果
        res = mysql_store_result(con);
        if (res)
        {
            //返回查询的行数
            result += "共" + to_string(mysql_num_rows(res)) + "条记录\n";

            char buff[100];
            sprintf(buff, "%-20s%-8s%-9s%-11s%-3s%-3s", "游戏时间", "学号", "姓名", "MAPID", "行", "列");
            result += buff;
            sprintf(buff, "%-6s%-5s%-7s%-9s%-9s%-9s\n", "分数", "步数", "合成值", "时长(ms)", "结果", "得分");
            result += buff;

            for (int i = 0; i < mysql_num_rows(res); ++i)
            {
                res_row = mysql_fetch_row(res);
                if (res_row < 0)
                    break;

                sprintf(buff, "%-20s%-8s%-9s%-11s%-3s%-3s", res_row[0], res_row[1], res_row[2], res_row[3], res_row[4], res_row[5]);
                result += buff;
                sprintf(buff, "%-6s%-5s%-7s%-9s%-9s%-9s\n", res_row[6], res_row[7], res_row[8], res_row[9], res_row[10], res_row[11]);
                result += buff;
            }
        }
        else //res==NULL
        {
            if (mysql_field_count(con) == 0) //返回insert update delete 等影响的列数
                int num_rows = mysql_affected_rows(con);
            else
            {
                cout << "get result error: " << mysql_error(con) << endl;
                return "";
            }
        }
    }
    //释放结果集
    mysql_free_result(res);
    return result;
}