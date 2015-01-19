#include "alarm.h"
#include "ui_alarm.h"
#include "mainwindow.h"

#include <QFile>
#include <QFileInfo>

#include <cmath>

Alarm::Alarm(AlarmPersistence* alarmer, Config cfg, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Alarm)
    , alarmer(alarmer)
    , config(cfg)
{
    ui->setupUi(this);

    on_config_update();

    connect(alarmer, SIGNAL(timeout()), this, SLOT(on_timer()));
}

Alarm::~Alarm()
{
    delete ui;
}

void Alarm::on_h_plus_clicked()
{
    alarmer->increment_hour();
    ui->hour->setText(QString("%1").arg(alarmer->getAlarmPoint().first, 2, 10, QChar('0')));
}

void Alarm::on_h_min_clicked()
{
    alarmer->decrement_hour();
    ui->hour->setText(QString("%1").arg(alarmer->getAlarmPoint().first, 2, 10, QChar('0')));
}

void Alarm::on_m_plus_clicked()
{
    alarmer->increment_minute();
    ui->minute->setText(QString("%1").arg(alarmer->getAlarmPoint().second, 2, 10, QChar('0')));
}

void Alarm::on_m_min_clicked()
{
    alarmer->decrement_minute();
    ui->minute->setText(QString("%1").arg(alarmer->getAlarmPoint().second, 2, 10, QChar('0')));
}

void Alarm::on_alarm_stop_clicked()
{
    alarmer->reset_snooze();
    alarmer->stop_alarm();
}

void Alarm::on_snooze_clicked()
{
    alarmer->stop_alarm();
}

void Alarm::setAlarmActivity(bool active)
{
    if (active)
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

void Alarm::on_alarm_status_btn_clicked()
{
    alarmer->toggle_active();
    setAlarmActivity(alarmer->isActive());
    emit activity_toggle(alarmer->isActive());
}

void Alarm::on_timer()
{
    using namespace std;

    auto currentTime = alarmer->getCurrentTime();

    auto time = chrono::system_clock::to_time_t(currentTime);
    auto local = localtime(&time);

    auto hour = local->tm_hour;
    auto minute = local->tm_min;
    auto second = local->tm_sec;

    auto now = chrono::hours(hour) + chrono::minutes(minute) + chrono::seconds(second);
    auto soon = chrono::hours(alarmer->getNextAlarmPoint().first) + chrono::minutes(alarmer->getNextAlarmPoint().second);
    if (soon < now)
        soon += chrono::hours(24);

    auto diff = soon-now;

    auto diffHour = chrono::duration_cast <chrono::hours> (diff).count();
    auto diffMin = chrono::duration_cast <chrono::minutes> (diff - chrono::duration_cast <chrono::hours>(diff)).count();
    auto diffSec = chrono::duration_cast <chrono::seconds> (diff - chrono::duration_cast <chrono::minutes>(diff)).count();

    QString str =
    QString("%1:%2:%3")
        .arg(diffHour, 2, 10, QChar('0'))
        .arg(diffMin, 2, 10, QChar('0'))
        .arg(diffSec, 2, 10, QChar('0')
    );
    ui->remaining_time->setText(str);

    ui->time_label->setText(
        QString("%1:%2:%3")
            .arg(hour, 2, 10, QChar('0'))
            .arg(minute, 2, 10, QChar('0'))
            .arg(second, 2, 10, QChar('0')
        )
    );
}

void Alarm::on_vol_increase_clicked()
{
    auto vol = std::min(100, config.cget().volume + 5);
    config.get()()->volume = vol;
    ui->volume->setText(QString("%1").arg(vol, 3, 10, QChar('0')));
}

void Alarm::on_vol_decrease_clicked()
{
    auto vol = std::max(0, config.cget().volume - 5);
    config.get()()->volume = vol;
    ui->volume->setText(QString("%1").arg(vol, 3, 10, QChar('0')));
}

void Alarm::on_test_volume_clicked()
{
    alarmer->playTestSound();
}

void Alarm::on_vol_increase_2_clicked()
{
    close();
}

void Alarm::on_config_update()
{
    alarmer->load();

    // +1 -1 hour auto sets it
    on_h_plus_clicked();
    on_h_min_clicked();
    // +5 -5 minute auto sets it
    on_m_plus_clicked();
    on_m_min_clicked();

    ui->volume->setText(QString("%1").arg(config.cget().volume, 3, 10, QChar('0')));
    // load alarm activity from persistence class
    setAlarmActivity(alarmer->isActive());
}
