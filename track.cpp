#include "track.h"

#include <QFile>
#include <QFileInfo>

Track::Track(QString absolutePath, QString title, QString fileExtension)
    : absolutePath(absolutePath)
    , fileExtension(fileExtension)
    , title(title)
{

}
Track::Track(QString absolutePath)
    : absolutePath(absolutePath)
    , fileExtension(
        [&] {
            // extracts the fileExtension automatically
            auto temp = absolutePath.toStdString();
            QString result;
            for (auto rev = temp.rbegin(); rev != temp.rend() && *rev != '.'; ++rev)
                result.push_back(*rev);
            return result;
        }())
    , title(
        [&] {
            auto temp = QFileInfo(absolutePath).fileName();
            temp.chop(fileExtension.length() + 1);
            return temp;
        }()) // removes path and extension
{
}

