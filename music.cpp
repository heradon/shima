#include "music.h"
#include "ui_music.h"
#include "common_errors.h"

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileInfo>

#include <cmath>

Music::Music(Config cfg, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Music)
    , fsys()
    , player(&fsys)
    , trackListModel(new QStringListModel(this))
    , config(std::move(cfg))
    , enqueued{"", "", ""}
    , trackPositionUpdateTimer(new QTimer(this))
    , sliderBlock(false)
{
    ui->setupUi(this);

    ui->tracklist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tracklist->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tracklist->setModel(trackListModel);

    connect(ui->vol_down, SIGNAL(clicked()), this, SLOT(decrease_volume()));
    connect(ui->vol_up, SIGNAL(clicked()), this, SLOT(increase_volume()));
    connect(trackPositionUpdateTimer, SIGNAL(timeout()), this, SLOT(on_timer()));

    scan_tracks();

    trackPositionUpdateTimer->start(500);
}

Music::~Music()
{
    delete ui;
}

void Music::scan_tracks()
{
    // const QString pathBase = "./music";
    const QString pathBase = QString::fromStdString(config.cget().musicPath);

    try
    {
        QStringList filter;
        filter << "*.mp3";
        auto list = QDir(pathBase).entryList(filter);
        player.clear();
        for (auto const& i : list)
        {
            player.add((pathBase + "/" + i).toStdString());
        }
        trackListModel->setStringList(list);
    }
    catch (...)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Music path not found.\n") + pathBase);
        msgBox.exec();
    }
}

void Music::play_track()
{
    try
    {
        player.play();
        ui->slider->setMaximum(player.getLength());
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::pause_track()
{
    try
    {
        player.pause();
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::toggle_track_playback()
{
    try
    {
        player.togglePause();
        ui->slider->setMaximum(player.getLength());
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::next_track(bool circular)
{
    try
    {
        player.next(circular);
        ui->slider->setMaximum(player.getLength());
        ui->tracklist->setCurrentIndex(ui->tracklist->model()->index(player.getIndex(), 0));
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::previous_track()
{
    try
    {
        player.previous(true);
        ui->slider->setMaximum(player.getLength());
        ui->tracklist->setCurrentIndex(ui->tracklist->model()->index(player.getIndex(), 0));
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::load_track(int index)
{
    try
    {
        player.setIndex(index);
    }
    catch(FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::increase_volume()
{
    float vol = std::min(100.f, player.getVolume()*100.f + 5.f);
    player.setVolume(vol / 100.f);
}

void Music::decrease_volume()
{
    float vol = std::min(100.f, player.getVolume()*100.f - 5.f);
    player.setVolume(vol / 100.f);
}

void Music::on_rescan_button_clicked()
{
    scan_tracks();
}

void Music::on_tracklist_doubleClicked(const QModelIndex &index)
{
    on_tracklist_pressed(index);
    try
    {
        play_track();
    }
    catch (FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::on_tracklist_pressed(const QModelIndex &index)
{
    try
    {
        load_track(index.row());
    }
    catch (FMODSound::Error const& exc)
    {
        showCritical(exc);
    }
}

void Music::on_music_play_clicked()
{
    toggle_track_playback();
}

void Music::on_slider_sliderReleased()
{
    player.setPosition(ui->slider->value());
    sliderBlock = false;
}

void Music::on_timer()
{
    if (sliderBlock)
        return;

    try
    {
        ui->slider->setValue(player.getPosition());
        if (ui->slider->maximum() != 0 && ui->slider->maximum() == ui->slider->value())
        {
            next_track(false);
        }
    }
    catch (FMODSound::Error const& exc)
    {
        qCritical() << exc.what() << " occured: " << exc.resultString() << " - in " << exc.functionName.c_str();
    }
}

void Music::on_slider_sliderPressed()
{
    sliderBlock = true;
}

void Music::on_music_next_clicked()
{
    next_track(true);
}

void Music::on_music_forward_clicked()
{
    previous_track();
}

void Music::on_window_back_clicked()
{
    close();
}