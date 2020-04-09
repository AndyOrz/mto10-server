#include "option.h"
#include "cmdline.h"
#include "../common.h"

int main(int argc, char *argv[])
{
    Cmdline_Option option(argc, argv);

    cout << "base=" << option.base << endl;
    cout << "ipaddr=" << option.ipaddr << endl;
    cout << "stuno=" << option.stuno << endl;
    cout << "passwd=" << option.passwd << endl;
    cout << "mapid=" << option.mapid << endl;
    cout << "row=" << option.row << endl;
    cout << "col=" << option.col << endl;
    cout << "timeout=" << option.timeout << endl;
    cout << "stepping=" << option.stepping << endl;
}