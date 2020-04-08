#ifndef TCP_H
#define TCP_H

#include "../common.h"
using namespace std;

class TCPServer
{
private:
    int sock;
    int buffer_size;
    char *read_buffer, *write_buffer;
    int read_remaining, write_remaining;

protected:
    string ReadLine();            //异步读
    string ReadLine(timeval &ot); //超时读
    int WriteLine(string &s);     //同步写

public:
    TCPServer(int sock, int buffer_size);
    ~TCPServer();

    map<string, int> login(DatabaseAccess *db);
    int UserAuthenticate(string auth_str, DatabaseAccess *db);
    map<string, string> ReadBlock();
    map<string, string> ReadBlock(int overtime);
    int WriteBlock(map<string, string> &map, list<string> keylist);
};

#endif