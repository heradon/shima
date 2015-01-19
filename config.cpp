#include "config.h"

#include <string>
#include <fstream>
#include <streambuf>

#include <QMessageBox>

Config::Config(std::string const& fileName)
    : fileName_(fileName)
    , filePersistence_{new ConfigContent}
{
    load();
}

Config::~Config()
{
    save();
}

ConfigContentWrapper Config::get()
{
    return {this};
}

const ConfigContent Config::cget() const
{
    return *filePersistence_;
}
void Config::load()
{
    std::ifstream reader (fileName_);
    if (reader.good())
    {
        std::string str((std::istreambuf_iterator<char>(reader)),
                         std::istreambuf_iterator<char>());

        ConfigContent content;
        try {
            auto tree = JSON::parse_json(str);
            JSON::js_parse(content, "config", tree);
        }
        catch(std::exception& exc)
        {
            QMessageBox msgBox;
            msgBox.setText(QString("Config has been reset, it was corrupted.\n") + QString::fromStdString(exc.what()));
            msgBox.exec();

            save();
        }
        *filePersistence_.get() = content;
    }
    else
    {
        save();
    }
}

void Config::save()
{
    std::ofstream writer (fileName_);
    auto string = JSON::js_try_stringify("config", *filePersistence_.get());
    writer << "{\n\t\"config\":" << JSON::js_beautify(string, 1) << "\n}";
}

ConfigContentWrapper::ConfigContentWrapper(Config *parent)
    : parent_(parent)
{
}

ConfigContentWrapper::~ConfigContentWrapper()
{
    parent_->save();
}

std::shared_ptr<ConfigContent> ConfigContentWrapper::operator()()
{
    return parent_->filePersistence_;
}

ConfigContent::ConfigContent(std::string musicPath, std::string alarmFile, std::pair<short, short> alarmPoint, bool alarmActive, unsigned short volume, unsigned short snoozeTime)
    : musicPath(std::move(musicPath))
    , alarmFile(std::move(alarmFile))
    , alarmPoint(alarmPoint)
    , alarmActive(alarmActive)
    , volume(volume)
    , snoozeTime(snoozeTime)
{
}
