#ifndef SYSTEM_MENU_H
#define SYSTEM_MENU_H

#include <QDialog>
#include "config.h"

namespace Ui {
class SystemMenu;
}

class SystemMenu : public QDialog
{
    Q_OBJECT

public:
    explicit SystemMenu(Config cfg);
    ~SystemMenu();

private slots:
    void on_music_button_clicked();

    void on_music_button_2_clicked();

    void on_alarm_button_6_clicked();

private:
    Ui::SystemMenu *ui;
    Config config;
};

#endif // SYSTEM_MENU_H
