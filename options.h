#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include "config.h"

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(Config cfg);
    ~Options();

private slots:
    void on_outputSelector_currentIndexChanged(const QString &arg1);

    void on_alarm_button_6_clicked();

private:
    Ui::Options *ui;
    Config config;
};

#endif // OPTIONS_H
