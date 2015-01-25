#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <QDialog>

namespace Ui {
class SystemInfo;
}

class SystemInfo : public QDialog
{
    Q_OBJECT

public:
    explicit SystemInfo(QWidget *parent = 0);
    ~SystemInfo();

private:
    Ui::SystemInfo *ui;
};

#endif // SYSTEM_INFO_H
