#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

#ifndef Q_MOC_RUN
#   include "json_api/parse/jsd.h"
#   include "json_api/parse/jsd_convenience.h"
#   include "json_api/stringify/jss.h"
#   include "json_api/stringify/jss_fusion_adapted_struct.h"
#endif

#include <string>
#include <memory>

class Config;

struct ConfigContent : public JSON::FusionStruct <ConfigContent>
                     , public JSON::ParsableStruct <ConfigContent>
{
    std::string musicPath;
    std::string alarmFile;
    std::pair <short, short> alarmPoint;
    bool alarmActive;
    unsigned short volume;
    unsigned short snoozeTime;
    unsigned short musicVolume;
    std::string deviceName;
    unsigned short remotePort;
    bool startPyServer;
    std::string pyFileName;
    std::string shipyDevicePrefix;
    std::string pythonExecutable;

    ConfigContent(std::string musicPath = "./music", std::string alarmFile = "./sinewave.mp3",
                  std::pair <short, short> alarmPoint = {0, 0}, bool alarmActive = false, unsigned short volume = 80,
                  unsigned short snoozeTime = 5, unsigned short musicVolume = 50, std::string deviceName = "AUTODETECT",
                  unsigned short remotePort = 9999, bool startPyServer = false,
                  std::string pyFileName = "shipy.py",
                  std::string shipyDevicePrefix = "",
                  std::string pythonExecutable = "");
};

namespace __INTERNAL
{
    using AlarmPointT = std::pair <short, short>;
}

BOOST_FUSION_ADAPT_STRUCT
(
    ConfigContent,
    (std::string, musicPath)
    (std::string, alarmFile)
    (__INTERNAL::AlarmPointT, alarmPoint)
    (bool, alarmActive)
    (unsigned short, volume)
    (unsigned short, snoozeTime)
    (unsigned short, musicVolume)
    (std::string, deviceName)
    (unsigned short, remotePort)
    (bool, startPyServer)
    (std::string, pyFileName)
    (std::string, shipyDevicePrefix)
    (std::string, pythonExecutable)
)

class ConfigContentWrapper
{
public:
   ConfigContentWrapper(Config* parent);
    ~ConfigContentWrapper();

   ConfigContentWrapper& operator=(ConfigContentWrapper const&) = delete;
   ConfigContentWrapper(ConfigContentWrapper const&) = delete;

   std::shared_ptr <ConfigContent> operator()(); // const would be a lie

private:
    Config* parent_;
};

class Config
{
    friend class ConfigContentWrapper;

public:
    Config(std::string const& fileName);
    ~Config();

    Config& operator=(Config const& rhs) = default;
    Config(Config const& rhs) = default;

    ConfigContentWrapper get(); // const would be a lie
    const ConfigContent cget() const;

    void load();
    void save();

private:
    std::string fileName_;
    std::shared_ptr <ConfigContent> filePersistence_;
};

#endif // CONFIG_H
