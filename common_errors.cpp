#include "common_errors.h"

#include <QDebug>
#include <QMessageBox>

#include <string>
#include <sstream>

void showCritical(const FMODSound::Error &exc)
{
    qCritical() << exc.what() << " occured: " << exc.resultString() << " - in " << exc.functionName.c_str();

    QMessageBox msgBox;
    auto iter = commonFmodErrors.find(exc.result);
    if (iter != std::end(commonFmodErrors))
        msgBox.setText(QString(iter->second).leftJustified(200, ' '));
    else
        msgBox.setText("An unknown error occured");

    std::stringstream sstr;
    sstr << exc.what() << " occured: " << exc.resultString() << " - in " << exc.functionName.c_str();

    msgBox.setDetailedText(QString::fromStdString(sstr.str()));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}
