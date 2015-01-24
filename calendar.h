#ifndef CALENDAR_H
#define CALENDAR_H

#include <QDialog>

namespace Ui {
class Calendar;
}

class Calendar : public QDialog
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = 0);
    ~Calendar();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Calendar *ui;
};

#endif // CALENDAR_H
