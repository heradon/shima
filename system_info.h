#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <QDialog>

namespace Ui {
class system_info;
}

class system_info : public QDialog
{
    Q_OBJECT

public:
    explicit system_info(QWidget *parent = 0);
    ~system_info();

private:
    Ui::system_info *ui;
};

#endif // SYSTEM_INFO_H
