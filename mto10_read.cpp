#include "mto10_read.h"

int main(int argc, char *argv[])
{
    // Init CmdLine
    Cmdline_Option option(argc, argv);

    //Init ReadConfig
    ConfigManager cm("server.conf");

    //Init Database
    DatabaseAccess db;
    char* buff=new char[100];

    bzero(buff,100);
    cm.GetString("database","db_host",buff,100,"");
    string db_host=buff;

    bzero(buff,100);
    cm.GetString("database","username",buff,100,"");
    string db_username=buff;

    bzero(buff,100);
    cm.GetString("database","password",buff,100,"");
    string db_password=buff;

    bzero(buff,100);
    cm.GetString("database","db_name",buff,100,"");
    string db_name=buff;

    bzero(buff,100);
    cm.GetString("database","charset",buff,100,"");
    string db_charset=buff;

    db.Init_DB(db_host,db_username,db_password,db_name,db_charset);

    cout<<db.SelectStest(option.mapid,option.row,option.col,option.limit)<<endl;
}