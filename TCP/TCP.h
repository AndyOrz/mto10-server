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
    string ReadLine();        //异步读
    int WriteLine(string &s); //同步写

public:
    TCPServer(int sock, int buffer_size);
    ~TCPServer();

    map<string,int> login();
	int UserAuthenticate(string auth_str);
    map<string, string> ReadBlock();
    int WriteBlock(map<string, string> &map, list<string> keylist);
};
