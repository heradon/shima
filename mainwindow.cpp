#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "music.h"
#include "alarm.h"
#include "calendar.h"

#include <cstring>
#include <string>
#include <memory>
#include <QFile>

MainWindow::MainWindow(Config cfg, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , alarmer(new AlarmPersistence(cfg, this))
    , timer(new QTimer(this))
    , config(std::move(cfg))
    , configRefresh(new QTimer(this))
    , alarmWindow(new Alarm(alarmer, config, this))
{
    ui->setupUi(this);

    // External Stylesheet for customize by Users

    QFile File("stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    qApp->setStyleSheet(StyleSheet);
    //setStyleSheet(StyleSheet);


    set_alarm_status(config.cget().alarmActive);

    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    connect(configRefresh, SIGNAL(timeout()), this, SLOT(refresh_config()));
    connect(alarmWindow, SIGNAL(activity_toggle(bool)), this, SLOT(set_alarm_status(bool)));

    timer->start(1000);
    configRefresh->start(10000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh_config()
{
    config.load();
    alarmWindow->on_config_update();
}

void MainWindow::on_timer()
{
    auto currentTime = alarmer->getCurrentTime();

    auto time = std::chrono::system_clock::to_time_t(currentTime);
    auto local = localtime(&time);

    QString timeString;
    timeString.sprintf("%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
    ui->uhrzeit->setText(timeString);
}

void MainWindow::set_alarm_status(bool status)
{
    if (status)
    {
        ui->alarm_status->setText("ON");
        ui->alarm_status->setStyleSheet("color: rgb(0, 255, 0);");
    }
    else
    {
        ui->alarm_status->setText("OFF");
        ui->alarm_status->setStyleSheet("color: rgb(255, 0, 0);");
    }
}

void MainWindow::on_music_button_clicked()
{
    std::unique_ptr<Music> musicWindow (new Music (config));
    musicWindow->setModal(true);
    musicWindow->exec();
}

void MainWindow::on_alarm_button_clicked()
{
    alarmWindow->exec();
}

void MainWindow::on_calendar_button_clicked()
{
    std::unique_ptr<Calendar> calendarWindow (new Calendar);
    calendarWindow->setModal(true);
    calendarWindow->exec();
}
