#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "alarmpersistence.h"
#include "alarm.h"

#include <QMainWindow>
#include <QTimer>
#include <chrono>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Config cfg, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_timer();
    void set_alarm_status(bool status);
    void refresh_config();

private slots:
    void on_music_button_clicked();

    void on_alarm_button_clicked();

    void on_calendar_button_clicked();

private:
    Ui::MainWindow *ui;
    AlarmPersistence* alarmer;
    QTimer* timer;
    Config config;
    QTimer* configRefresh;
    Alarm* alarmWindow;
};

#endif // MAINWINDOW_H
