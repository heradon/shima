#ifndef DIRECTORYLISTMODEL_H
#define DIRECTORYLISTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QFileInfo>
#include <QStringList>
#include <QItemSelectionModel>

class DirectoryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    int rowCount(QModelIndex const& = QModelIndex()) const;
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;
    QFileInfo getFileInfo(QModelIndex const& index) const;
    QFileInfo getFileInfo(int row) const;
    int getDirectoryCount() const;

    void loadDirectory(QString jail);
    void changeDirectory(QModelIndex const& index);

    DirectoryListModel(QStringList filter);
    ~DirectoryListModel() = default;

private:
    void changeDirectory(QString path);

private:
    std::vector <QFileInfo> currentList_;
    QString currentDirectory_;
    QString jail_;
    QStringList filter_;
    int dirCount_;
};

#endif // DIRECTORYLISTMODEL_H
