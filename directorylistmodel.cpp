#include "directorylistmodel.h"

#include <QDir>
#include <QFile>

int DirectoryListModel::rowCount(QModelIndex const&) const
{
    // TODO
    return currentList_.size();
}

QVariant DirectoryListModel::data(QModelIndex const& index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    // TODO
    std::size_t i = index.row();
    if (i == 0 && currentDirectory_ != jail_)
    {
        return QString("[ .. ]");
    }
    if (i < currentList_.size())
    {
        QString str;
        if (currentList_[i].isDir())
        {
            str = "[ ";
            str += currentList_[i].fileName();
            str += " ]";
        }
        else
        {
            auto removeExtension = [](QString str) -> QString
            {
                for (int i = 0; i != str.length(); ++i)
                {
                    if (str[str.length() - i] == '.')
                    {
                        str.chop(i);
                        return str;
                    }
                    if (str[str.length() - i] == '/')
                        return str;
                }
                return str;
            };
            str += removeExtension(currentList_[i].fileName());
        }
        return str;
    }
    else
        return QVariant();
}

QFileInfo DirectoryListModel::getFileInfo(const QModelIndex &index) const
{
    std::size_t i = index.row();
    return getFileInfo(i);
}

QFileInfo DirectoryListModel::getFileInfo(int row) const
{
    if (row >= 0 && row < (int)currentList_.size())
    {
        return currentList_[row];
    }
    return QFileInfo();
}

int DirectoryListModel::getDirectoryCount() const
{
    return dirCount_;
}

void DirectoryListModel::loadDirectory(QString jail)
{
    jail_ = jail;
    currentDirectory_ = jail_;
    changeDirectory(jail);
}

void DirectoryListModel::changeDirectory(const QModelIndex &index)
{
    std::size_t i = index.row();
    if (i == 0 && currentDirectory_ != jail_)
    {
        if (!currentList_[i].isDir())
            return;
        changeDirectory(currentList_[i].absolutePath());
    }
    else if (i < currentList_.size())
    {
        auto elem = currentList_[i];
        if (!elem.isDir())
           return;
        changeDirectory(elem.filePath());
    }
}

DirectoryListModel::DirectoryListModel(QStringList filter)
    : filter_(filter)
{
}

void DirectoryListModel::changeDirectory(QString path)
{
    currentDirectory_ = path;
    currentList_.clear();
    if (path != jail_)
    {
        currentList_.push_back(QFileInfo(currentDirectory_));
        dirCount_ = 1;
    }
    else
        dirCount_ = 0;

    auto dir = QDir(path);
    for (auto const& i : dir.entryInfoList(filter_, QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs, QDir::DirsFirst))
    {
        if (i.isDir())
            dirCount_++;
        currentList_.push_back(i);
    }
    emit dataChanged(createIndex(0, 0), createIndex(currentList_.size() + 1, 0));
}
