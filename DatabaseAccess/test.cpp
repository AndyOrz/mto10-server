#include "DatabaseAccess.h"
using namespace std;

int main()
{
    DatabaseAccess db;
    db.init_db("localhost","mto10","Anqi_990321","hw_mto10");
    db.exec("select * from student");
    //db.exec("insert into stu_info values('000002','hualian','21','1005',100)");
    return 0;
}