#ifndef ALARM_H
#define ALARM_H

#include "alarmpersistence.h"

#include <memory>
#include <QDialog>

namespace Ui {
class Alarm;
}

class Alarm : public QDialog
{
    Q_OBJECT

public:
    explicit Alarm(AlarmPersistence* alarmer, Config cfg, QWidget* parent = nullptr);
    ~Alarm();

signals:
    void activity_toggle(bool set);

public slots:
    void on_config_update();

private slots:
    void on_h_plus_clicked();
    void on_h_min_clicked();
    void on_m_plus_clicked();
    void on_m_min_clicked();
    void on_alarm_stop_clicked();
    void on_snooze_clicked();
    void on_alarm_status_btn_clicked();
    void on_timer();
    void on_vol_increase_clicked();
    void on_vol_decrease_clicked();
    void on_test_volume_clicked();
    void on_vol_increase_2_clicked();

private:
    void setAlarmActivity(bool active);

private:
    Ui::Alarm* ui;
    AlarmPersistence* alarmer;
    Config config;
};

#endif // ALARM_H
