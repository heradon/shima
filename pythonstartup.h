#ifndef PYTHONSTARTUP
#define PYTHONSTARTUP

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QObject>
#include <memory>

class PythonProcess : public QObject {

    Q_OBJECT

private:
    PythonProcess();

public:
    ~PythonProcess() = default; // default dtor

    // disallow copying
    PythonProcess& operator=(PythonProcess const&) = delete;
    PythonProcess(PythonProcess const&) = delete;

    static PythonProcess& getInstance();
    void start(QString fileName, QStringList arguments);
    bool isRunning() const;

private slots:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void error(QProcess::ProcessError error);
    void started();
    void stop();

private:
    std::unique_ptr<QProcess> process_;
    bool running_;
};

#endif // PYTHONSTARTUP
