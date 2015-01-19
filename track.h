#ifndef TRACK_H
#define TRACK_H

#include <QString>

struct Track
{
    QString absolutePath;
    QString fileExtension;
    QString title;

    Track(QString absolutePath, QString title, QString fileExtension);
    Track(QString absolutePath);
};

#endif // TRACK_H
