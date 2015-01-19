#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef __linux__
    auto pathString = QDir::homePath() + "/.config/shima";

    if (!QDir(pathString).exists())
        QDir(pathString).mkdir(".");

    Config configOrigin((pathString + "/config.json").toStdString());

    a.setOverrideCursor( QCursor( Qt::BlankCursor ) );
#else
    Config configOrigin("./config.json");
#endif
    if (!QFile("./stylesheet.qss").exists())
        QFile::copy(":/intern/stylesheet.qss", "./stylesheet.qss");

    MainWindow w(configOrigin);
    w.show();

    return a.exec();
}
