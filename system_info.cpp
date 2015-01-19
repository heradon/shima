#include "system_info.h"
#include "ui_system_info.h"

system_info::system_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::system_info)
{
    ui->setupUi(this);
}

system_info::~system_info()
{
    delete ui;
}
