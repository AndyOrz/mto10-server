#include "Tools.h"

Tools::Tools(/* args */)
{
}

Tools::~Tools()
{
}

string Tools::Get_Time()
{
	time_t t;
	time(&t);
	struct tm *tt;
	tt = localtime(&t);
	string timebuf(to_string(tt->tm_year + 1900)+"-"+to_string(tt->tm_mon + 1)+"-"+to_string(tt->tm_mday)
        +" "+to_string(tt->tm_hour)+":"+to_string(tt->tm_min)+":"+to_string(tt->tm_sec));
    return timebuf;
}