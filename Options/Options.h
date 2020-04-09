#ifndef Options_H
#define Options_H

#include"cmdline.h"

class Cmdline_Option
{
public:
    bool stest;
    int limit;
    int mapid;
    int row;
    int col;
    
    Cmdline_Option(int argc, char *argv[]);
    std::string Usage(int argc, char *argv[]);
};

#endif