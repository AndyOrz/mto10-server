// #include "../md5/md5.h"
// #include "tcp.h"
#include "../common.h"

int main()
{
    srand((unsigned)time(0));
	string randstr="";
	char c[10];
	for (int i=0;i<5;i++)
	{
		sprintf(c,"%08x",rand());
		randstr+=c;
	}
    printf("%s\n",randstr.c_str());
    printf("%d\n",(int)randstr.length());
}