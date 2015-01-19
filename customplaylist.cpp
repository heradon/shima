#include "customplaylist.h"

#include <stdexcept>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QUrl>

CustomPlaylist::CustomPlaylist(QObject *parent) : QObject(parent)
{

}

CustomPlaylist::~CustomPlaylist()
{

}

void CustomPlaylist::load_from_directory(const QString &directory, QStringList const& filter)
{
    QDir dir(directory);
    if (!dir.exists())
        throw std::invalid_argument("directory does not exist");

    playlist.clear();

    auto tempList = dir.entryList(filter, QDir::Files);
    for (auto elem : tempList)
        playlist.push_back(Track(QFileInfo(directory + "/" + elem).absoluteFilePath()));

    emit on_list_load_finished(playlist.size());
}

QStringList CustomPlaylist::getTitles() const
{
    QStringList list;
    for (auto const& track : playlist)
    {
        list.push_back(track.title);
    }
    return list;
}

bool CustomPlaylist::isLast() const
{
    return playlistPosition == cend() - 1;
}

int CustomPlaylist::getIndex() const
{
    return playlistPosition - cbegin();
}

void CustomPlaylist::next(bool shallPlay)
{
    if (playlist.empty())
        return;

    playlistPosition++;
    if (playlistPosition == std::end(playlist))
        playlistPosition = std::begin(playlist);

    emit on_track_change(*playlistPosition, shallPlay);
}

void CustomPlaylist::previous(bool shallPlay)
{
    if (playlist.empty())
        return;

    playlistPosition--;
    if (playlistPosition == std::begin(playlist) - 1)
        playlistPosition = std::end(playlist) - 1;

    emit on_track_change(*playlistPosition, shallPlay);
}

void CustomPlaylist::set_index(int index, bool shallPlay)
{
    if (index >= (signed int)playlist.size() || index < 0)
        throw std::out_of_range("index out of bounds");
    playlistPosition = std::begin(playlist) + index;

    emit on_track_change(*playlistPosition, shallPlay);
}

int CustomPlaylist::getSize() const
{
    return playlist.size();
}

std::vector <Track>::iterator CustomPlaylist::begin()
{
    return std::begin(playlist);
}

std::vector <Track>::iterator CustomPlaylist::end()
{
    return std::end(playlist);
}

std::vector <Track>::const_iterator CustomPlaylist::cbegin() const
{
    return playlist.cbegin();
}

std::vector <Track>::const_iterator CustomPlaylist::cend() const
{
    return playlist.cend();
}

