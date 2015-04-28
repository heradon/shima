#include "system_info.h"
#include "ui_system_info.h"
//#include "sys/types.h"
//#include "sys/sysinfo.h"

SystemInfo::SystemInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemInfo)
{
    ui->setupUi(this);
}

SystemInfo::~SystemInfo()
{
    delete ui;
}

/* struct sysinfo memInfo;

sysinfo (&memInfo);
long long totalVirtualMem = memInfo.totalram;

totalVirtualMem += memInfo.totalswap;
totalVirtualMem *= memInfo.mem_unit;
*/
// http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
