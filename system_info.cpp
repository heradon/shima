#include "system_info.h"
#include "ui_system_info.h"

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
