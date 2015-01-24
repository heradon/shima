#ifndef MUSIC_H
#define MUSIC_H

#include "config.h"
#include "customplaylist.h"
#include "fmodwrap.h"
#include "directorylistmodel.h"

#include <QDialog>
#include <QStringListModel>
#include <QTimer>

namespace Ui {
class Music;
}

class Music : public QDialog
{
    Q_OBJECT

public:
    explicit Music(Config cfg, QWidget *parent = 0);
    ~Music();

public slots:
    void scan_tracks();
    void play_track();
    void pause_track();
    void suspend_track();
    void unsuspend_track();
    void toggle_track_playback();
    void next_track(bool circular = false);
    void previous_track();
    void load_track(int index);
    void increase_volume();
    void decrease_volume();

private slots:
    void on_rescan_button_clicked();
    void on_tracklist_doubleClicked(const QModelIndex &index);
    void on_music_play_clicked();
    void on_tracklist_pressed(const QModelIndex &index);
    void on_slider_sliderReleased();
    void on_timer();
    void on_slider_sliderPressed();
    void on_music_next_clicked();
    void on_music_forward_clicked();
    void on_window_back_clicked();
    void on_enter_clicked();

private: // private member functions
    void player_rescan();

private: // private members
    Ui::Music *ui;
    FMODSound::System fsys;
    FMODSound::MediaPlayer player;
    DirectoryListModel trackListModel;
    QStringList realTrackList;
    Config config;
    Track enqueued;
    QTimer* trackPositionUpdateTimer;
    bool sliderBlock;
    bool suspended;
};

#endif // MUSIC_H
