#include "../common.h"
#include "cmdline.h"
#include "Options.h"

Cmdline_Option::Cmdline_Option(int argc, char *argv[])
{
    cmdline::parser a;
    a.add("help");
    a.add("stest");
    a.add<int>("mapid", '\0', "", false, -1);
    a.add<int>("row", '\0', "", false, -1);
    a.add<int>("col", '\0', "", false, -1);
    a.add<int>("limit", '\0', "", false, -1);

    a.parse_check(argc, argv);

    if (!a.exist("stest") || a.exist("help"))
    {
        if (!a.exist("stest"))
            cout << "必须指定参数[--gtest]" << endl;
        cout << Usage(argc, argv) << endl;
        exit(0);
    }

    stest = true;
    limit = a.get<int>("limit");
    mapid = a.get<int>("mapid");
    row = a.get<int>("row");
    col = a.get<int>("col");
}

std::string Cmdline_Option::Usage(int argc, char *argv[])
{
    printf("%s%s\n", getcwd(NULL, 0), argv[0]);
    string usage = "";
    usage += "Usage: mto10_read --help\n";
    usage += "       mto10_read --stest [--mapid xxx --row xx --col xx --limit xx]\n";
    usage += "e.g.  :\n";
    usage += "       mto10_read --stest                                             : 显示最新的10局游戏的结果(非竞赛)\n";
    usage += "       mto10_read --stest --limit 64                                  : 显示最新的64局游戏的结果(非竞赛)\n";
    usage += "       mto10_read --stest --mapid 12345                               : 显示最新的10局[mapid=12345/行列值任意]游戏的结果(非竞赛)\n";
    usage += "       mto10_read --stest --mapid 12345 --limit 32                    : 显示最新的32局[mapid=12345/行列值任意]游戏的结果(非竞赛)\n";
    usage += "       mto10_read --stest --mapid 12345 --row 8 --col 10              : 显示最新的10局[mapid=12345/行=8/列=10]游戏的结果(非竞赛)\n";
    usage += "       mto10_read --stest --mapid 12345 --row 8 --col 10 --limit 16   : 显示最新的16局[mapid=12345/行=8/列=10]游戏的结果(非竞赛)\n";
    return usage;
}