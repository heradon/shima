#include "system_menu.h"
#include "ui_system_menu.h"

system_menu::system_menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::system_menu)
{
    ui->setupUi(this);
}

system_menu::~system_menu()
{
    delete ui;
}
