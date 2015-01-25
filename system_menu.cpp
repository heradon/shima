#include "system_menu.h"
#include "ui_system_menu.h"
#include "system_info.h"
#include "options.h"

#include <memory>

SystemMenu::SystemMenu(Config cfg)
    : QDialog(nullptr)
    , ui(new Ui::SystemMenu)
    , config(cfg)
{
    ui->setupUi(this);
}

SystemMenu::~SystemMenu()
{
    delete ui;
}

void SystemMenu::on_music_button_clicked()
{
    std::unique_ptr <SystemInfo> info (new SystemInfo());
    info->exec();
}

void SystemMenu::on_music_button_2_clicked()
{
    std::unique_ptr <Options> opts (new Options(config));
    opts->exec();
}

void SystemMenu::on_alarm_button_6_clicked()
{
    close();
}
