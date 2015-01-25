#include "alarmpersistence.h"
#include "common_errors.h"

#include <QFileInfo>
#include <QDebug>
#include <QApplication>
#include <QMessageBox>

#include <string>
#include <sstream>
#include <fstream>

// DEBUG!
const int deltaMinute = 5;

AlarmPersistence::AlarmPersistence(Config cfg, QObject *parent)
    : QObject(parent)
    , timer(new QTimer(this))
    , currentTime()
    , alarmPoint()
    , snoozeOffset()
    , config(std::move(cfg))
    , active(false)
    , inSnoozeMode(false)
    , fsys()
    , alarmSound()
    , testSound()
{
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));

    load();

    try {
        if (config.cget().deviceName != "AUTODETECT")
            fsys.setOutput(FMODSound::stringToOutputType(config.cget().deviceName));
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }

    if (!QFile::exists("./sinewave.mp3"))
        QFile::copy(":/sounds/sinewave.mp3", "./sinewave.mp3");

    try {
        testSound = std::static_pointer_cast <FMODSound::Sound> (fsys.createSound(QFileInfo("./sinewave.mp3").absoluteFilePath().toStdString()));
        alarmSound = std::static_pointer_cast <FMODSound::Sound> (fsys.createStream(QFileInfo(QString::fromStdString(config.cget().alarmFile)).absoluteFilePath().toStdString()));
    }
    catch (FMODSound::Error const& exc) {
        showCritical(exc);

        // force exit
        exit(1);
    }

    timer->start(1000);
}

AlarmPersistence::~AlarmPersistence()
{
    timer->stop();
}

void AlarmPersistence::on_timer()
{
    currentTime = std::chrono::system_clock::now();

    auto time = std::chrono::system_clock::to_time_t(currentTime);
    auto local = localtime(&time);

    auto hour = local->tm_hour;
    auto minute = local->tm_min;
    auto second = local->tm_sec;

    // exact match: this is when you should start waking this lazy ass ;)
    if (active)
    {
        if (hour == alarmPoint.first && minute == alarmPoint.second && !inSnoozeMode && second == 0)
        {
            start_alarm();
            snooze();
        }
        // ok already in the snooze section?
        else if (inSnoozeMode)
        {
            auto curTime = std::chrono::hours(hour) + std::chrono::minutes(minute) + std::chrono::seconds(second);
            auto originalPoint = std::chrono::hours(alarmPoint.first) + std::chrono::minutes(alarmPoint.second);
            auto diff = (originalPoint + snoozeOffset) - curTime;

            if (std::chrono::duration_cast <std::chrono::seconds> (diff).count() <= 0)
            {
                start_alarm();
                increment_snooze();
            }
        }
    }

    emit timeout();
}

void AlarmPersistence::start_alarm()
{
    // play Sound
    try {
        auto play = [&]()
        {
            alarmSound->play(true);
            alarmSound->setVolume(static_cast <float> (config.cget().volume) / 100.f);
            alarmSound->unpause();
        };

        fsys.update();
        if (alarmSound->isPersistent())
        {
            if (alarmSound->isPlaying())
            {
                alarmSound->stop();
                play();

            }
            else if (alarmSound->isPaused())
            {
                alarmSound->unpause();
            }
            else
            {
                play();
            }
        }
        else
        {
            play();
        }
    } catch (FMODSound::Error const& exc)
    {
        showCritical(exc);

        // force exit
        exit(1);
    }
    emit alarm_fired();
}

void AlarmPersistence::save()
{
    auto&& content = config.get();
    content()->alarmPoint = alarmPoint;
    content()->alarmActive = active;
}

void AlarmPersistence::load()
{
    auto&& content = config.get();

    active = content()->alarmActive;
    alarmPoint = content()->alarmPoint;
}

void AlarmPersistence::increment_snooze()
{
    snoozeOffset += std::chrono::minutes(config.cget().snoozeTime);
}

void AlarmPersistence::snooze()
{
    if (!inSnoozeMode)
        increment_snooze();
    inSnoozeMode = true;
}

void AlarmPersistence::reset_snooze()
{
    snoozeOffset = std::chrono::seconds(0);
    inSnoozeMode = false;
}

void AlarmPersistence::increment_hour()
{
    alarmPoint.first++;
    if (alarmPoint.first >= 24)
        alarmPoint.first = 0;
    save();
}

void AlarmPersistence::increment_minute()
{
    alarmPoint.second += deltaMinute;
    if (alarmPoint.second == 60)
        alarmPoint.second = 0;
    save();
}

void AlarmPersistence::decrement_hour()
{
    if (alarmPoint.first == 0)
        alarmPoint.first = 24;
    alarmPoint.first--;
    save();
}

void AlarmPersistence::decrement_minute()
{
    if (alarmPoint.second == 0)
        alarmPoint.second = 60;
    alarmPoint.second -= deltaMinute;
    save();
}

void AlarmPersistence::toggle_active()
{
    active = !active;
    save();
}

std::chrono::time_point<std::chrono::system_clock> AlarmPersistence::getCurrentTime() const
{
    return currentTime;
}

void AlarmPersistence::setAlarmPoint(short hour, short minute)
{
    alarmPoint = std::make_pair (hour, minute);
}

void AlarmPersistence::setAlarmPoint(std::pair <short, short> const& point)
{
    alarmPoint = point;
}

std::pair<short, short> AlarmPersistence::getNextAlarmPoint() const
{
    auto next = std::chrono::hours(alarmPoint.first) + std::chrono::minutes(alarmPoint.second) + snoozeOffset;
    if (std::chrono::duration_cast <std::chrono::hours> (next).count() >= 24)
    {
        next -= std::chrono::hours(24);
    }
    return {
        std::chrono::duration_cast <std::chrono::hours> (next).count(),
        std::chrono::duration_cast <std::chrono::minutes> (next - std::chrono::duration_cast<std::chrono::hours>(next)).count()
    };
}

std::pair<short, short> AlarmPersistence::getAlarmPoint() const
{
    return alarmPoint;
}

bool AlarmPersistence::isActive() const
{
    return active;
}

void AlarmPersistence::playTestSound()
{
    try {
        testSound->play(true);
        testSound->setVolume(static_cast <float> (config.cget().volume) / 100.f);
        testSound->unpause();
    } catch (FMODSound::Error const& exc)
    {
        showCritical(exc);

        exit(1);
    }
}

void AlarmPersistence::stop_alarm()
{
    fsys.update();

    try {
        if (alarmSound->isOpened())
            if (alarmSound->isPlaying())
                alarmSound->stop();
    } catch (FMODSound::Error const& exc)
    {
        showCritical(exc);
        exit(1);
    }
    emit alarm_stopped();
}

