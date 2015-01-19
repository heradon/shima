#ifndef CUSTOMPLAYLIST_H
#define CUSTOMPLAYLIST_H

#include "track.h"

#include <QObject>
#include <QString>
#include <QStringList>

#include <vector>

class CustomPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit CustomPlaylist(QObject *parent = 0);
    ~CustomPlaylist();

signals:
    void on_track_change(Track, bool /* shall play */);
    void on_list_load_finished(int /* elements */);

public slots:
    void load_from_directory(QString const& directory, QStringList const& filter);
    void next(bool shallPlay = false);
    void previous(bool shallPlay = false);
    void set_index(int index, bool shallPlay = false);

public:
    int getSize() const;
    QStringList getTitles() const;
    bool isLast() const;
    int getIndex() const;

    std::vector <Track>::iterator begin();
    std::vector <Track>::iterator end();
    std::vector <Track>::const_iterator cbegin() const;
    std::vector <Track>::const_iterator cend() const;

private:
    std::vector <Track> playlist;
    std::vector <Track>::iterator playlistPosition;
};

#endif // CUSTOMPLAYLIST_H
