#ifndef SYSTEM_MENU_H
#define SYSTEM_MENU_H

#include <QDialog>

namespace Ui {
class system_menu;
}

class system_menu : public QDialog
{
    Q_OBJECT

public:
    explicit system_menu(QWidget *parent = 0);
    ~system_menu();

private:
    Ui::system_menu *ui;
};

#endif // SYSTEM_MENU_H
