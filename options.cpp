#include "options.h"
#include "ui_options.h"

Options::Options(Config cfg)
    : QDialog(nullptr)
    , ui(new Ui::Options)
    , config(cfg)
{
    ui->setupUi(this);

    ui->outputSelector->addItems(QStringList{
         "AUTODETECT",
         "NOSOUND",
         "NOSOUND_NRT",
         "DSOUND",
         "WINMM",
         "WASAPI",
         "ASIO",
         "PULSEAUDIO",
         "ALSA",
         "COREAUDIO",
         "AUDIOTRACK",
         "OPENSL",
         "AUDIOOUT",
         "MAX"
    });
}

Options::~Options()
{
    delete ui;
}

void Options::on_outputSelector_currentIndexChanged(const QString &arg1)
{
    config.get()()->deviceName = arg1.toStdString();
}

void Options::on_alarm_button_6_clicked()
{
    close();
}
