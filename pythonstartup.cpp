#include "pythonstartup.h"
#include <QDebug>

PythonProcess::PythonProcess()
    : process_(new QProcess)
{
    connect(process_.get(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(process_.get(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
}

PythonProcess& PythonProcess::getInstance() {
    static PythonProcess process;
    return process;
}

void PythonProcess::start(QString fileName, QStringList arguments)
{
    process_->start(fileName, arguments);
}

bool PythonProcess::isRunning() const
{
    return running_;
}

void PythonProcess::error(QProcess::ProcessError error)
{
    qDebug() << "Python Process Error: " << error;
}

void PythonProcess::started()
{
    running_ = true;
}

void PythonProcess::stop()
{
    process_->terminate();
}

void PythonProcess::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Process finished with code: " << exitCode << " and status: " << exitStatus;
    running_ = false;
}
