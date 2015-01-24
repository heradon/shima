#ifndef ALARMPERSISTENCE_H
#define ALARMPERSISTENCE_H

#include <QObject>
#include <QTimer>

#include "config.h"
#include "fmodwrap.h"

#include <utility>
#include <chrono>

class AlarmPersistence : public QObject
{
    Q_OBJECT
public:
    explicit AlarmPersistence(Config cfg, QObject *parent = 0);
    ~AlarmPersistence();

    std::chrono::time_point<std::chrono::system_clock> getCurrentTime() const;
    void setAlarmPoint(short hour, short minute);
    void setAlarmPoint(std::pair <short, short> const& point);
    std::pair <short, short> getNextAlarmPoint() const;
    std::pair <short, short> getAlarmPoint() const;
    bool isActive() const;
    void playTestSound() const;

signals:
    void timeout();
    void alarm_fired();
    void alarm_stopped();

public slots:
    void stop_alarm();
    void snooze();
    void reset_snooze();
    void increment_hour();
    void increment_minute();
    void decrement_hour();
    void decrement_minute();
    void toggle_active();
    void load();

private slots:
    void on_timer();

private:
    void start_alarm();
    void save();
    void increment_snooze();

private: // members
    QTimer* timer;
    std::chrono::time_point<std::chrono::system_clock> currentTime; //Aktuelle Uhrzeit für wecker (updated sich automatisch jede Sekunde)
    std::pair <short, short> alarmPoint; // hour, minute
    std::chrono::seconds snoozeOffset;
    Config config;
    bool active;
    bool inSnoozeMode;
    FMODSound::System fsys;
    std::shared_ptr<FMODSound::Sound> alarmSound;
    std::shared_ptr<FMODSound::Sound> testSound;
};

#endif // ALARMPERSISTENCE_H
