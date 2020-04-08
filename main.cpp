#include "main.h"

int main()
{
    //Init ReadConfig
    ConfigManager cm("server.conf");
    int port = cm.GetInt("default", "port", 1234);
    int default_delay = cm.GetInt("default","default_delay",5);
    int competition_delay = cm.GetInt("default","competition_delay",2);

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

    int serv_sock;
    pthread_t p_id;

    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1)
    {
        fprintf(stderr, "(%s)[Server]: 创建套接字失败\n", Tools::Get_Time().c_str());
        return 0;
    }

    struct sockaddr_in serv_addr;
    socklen_t clnt_addr_size = sizeof(struct sockaddr_in);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        fprintf(stderr, "(%s)[Server]: 套接字绑定失败\n", Tools::Get_Time().c_str());
        return 0;
    }

    int opt = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)))
    {
        fprintf(stderr, "(%s)[Server]: 设置端口复用失败\n", Tools::Get_Time().c_str());
        return 0;
    }

    if (listen(serv_sock, 1) == -1)
    {
        fprintf(stderr, "(%s)[Server]: 套接字监听失败\n", Tools::Get_Time().c_str());
        return 0;
    }

    printf("(%s)[Server]: 套接字初始化完成，等待连接 ...\n", Tools::Get_Time().c_str());
    fflush(stdout);

    while (1)
    {
        Thread_Args *targs = (Thread_Args *)malloc(sizeof(Thread_Args));
        if (!targs)
        {
            exit(0);
        }

        if ((targs->clnt_sock = accept(serv_sock, (struct sockaddr *)&targs->clnt_addr, &clnt_addr_size)) == -1)
        {
            printf("(%s)[Server]: accept失败\n", Tools::Get_Time().c_str());
            fflush(stdout);
            free(targs);
            continue;
        }
        targs->db=&db;
        targs->default_delay=default_delay;
        targs->competition_delay=competition_delay;
        pthread_create(&p_id, NULL, pthread_func, (void *)targs);
    }

    close(serv_sock);
    return 0;
}

void *pthread_func(void *args)
{
    pthread_detach(pthread_self());

    int clnt_sock = ((Thread_Args *)args)->clnt_sock;
    struct sockaddr_in clnt_addr = ((Thread_Args *)args)->clnt_addr;
    int default_delay = ((Thread_Args *)args)->default_delay;
    int competition_delay = ((Thread_Args *)args)->competition_delay;
    DatabaseAccess *db=((Thread_Args *)args)->db;
    free(args);

    char clnt_ip[20];
    int clnt_port;
    strcpy(clnt_ip, inet_ntoa(clnt_addr.sin_addr));
    clnt_port = ntohs(clnt_addr.sin_port);
    printf("(%s)[Server]: 建立连接[%s:%d]\n", Tools::Get_Time().c_str(), clnt_ip, clnt_port);
    fflush(stdout);

    TCPServer tcpserver(clnt_sock, 1000);
    map<string, int> gameinfo = tcpserver.login(db);
    if (gameinfo["GameType"] == -1)
    {
        printf("登录失败\n");
        return nullptr;
    }
    if (gameinfo["GameType"] == 0) //base模式
    {
        Game_Engine game;
        map<string, string> toClientBlock = game.InitGame(gameinfo, default_delay);
        printf("delay=%d\n",game.GetDelay());
        tcpserver.WriteBlock(toClientBlock, {"Type", "Content", "Row", "Col", "GameID", "Delay", "Map"});
        while (1)
        {
            map<string, string> fromClientBlock = tcpserver.ReadBlock(game.GetDelay());
            map<string, string> toClientBlock = game.Play(fromClientBlock);

            if (toClientBlock["Content"] == "MergeSucceeded")
            {
                tcpserver.WriteBlock(toClientBlock, {"Type", "Content", "GameID", "Step", "Score", "MaxValue", "OldMap", "NewMap"});
            }
            else if (toClientBlock["Content"] == "MergeFailed")
            {
                tcpserver.WriteBlock(toClientBlock,{"Type", "Content", "GameID","Step","Score","MaxValue"});              
                if (db->GameOver_Write(gameinfo,toClientBlock)==-1)
                    printf("%s WriteDatabaseFailed!\n",Tools::Get_Time().c_str());
                break;
            }
            else if (toClientBlock["Content"] == "GameOver" || toClientBlock["Content"] == "GameFinished" || toClientBlock["Content"] == "GameTimeout")
            {
                toClientBlock["FinalStep"] = toClientBlock["Step"];
                toClientBlock["FinalScore"] = toClientBlock["Score"];
                toClientBlock["FinalMaxValue"] = toClientBlock["MaxValue"];
                toClientBlock["FinalMap"] = toClientBlock["NewMap"];
                tcpserver.WriteBlock(toClientBlock, {"Type", "Content", "GameID", "FinalStep", "FinalScore", "FinalMaxValue", "FinalMap"});            
                if (db->GameOver_Write(gameinfo,toClientBlock)==-1)
                    printf("%s WriteDatabaseFailed!\n",Tools::Get_Time().c_str());
                break;
            }
            else
            {
                printf("异常错误！\n");
                break;
            }
        }
    }
    if (gameinfo["GameType"] == 1) //competition模式
    {
    }

    
    return nullptr;
}