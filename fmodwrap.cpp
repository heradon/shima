#include "fmodwrap.h"

#define ENUM_BEGIN(name) enum struct name : size_t {
#define ENUM_VALUE(name) name,
#define ENUM_END() };
#include "fmodresult.h"

#define ENUM_BEGIN(name) constexpr char const* const name##_strings[] = {
#define ENUM_VALUE(name) #name,
#define ENUM_END() };
#include "fmodresult.h"

// und jetzt noch die eigentliche Funktion
inline constexpr char const* toString(ResultEnum x)
{
    return ResultEnum_strings[static_cast<size_t>(x)];
}

#if BOOST_VERSION < 105600
    #define OPT_GET get
#else
    #define OPT_GET value
#endif

#include <stdexcept>
#include <fstream>
#include <cstring>
#include <functional>
#include <unordered_map>

std::unordered_map <FMOD_CHANNEL*, std::shared_ptr <FMODSound::Sound>> channelSoundLookupTable;
//------------------------------------------------------------------------------------------
FMOD_RESULT F_CALLBACK channelCallback(FMOD_CHANNELCONTROL *channel,  FMOD_CHANNELCONTROL_TYPE controltype,
                                       FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype, void*, void*)
{
    if (controltype != FMOD_CHANNELCONTROL_CHANNEL)
        return FMOD_OK;

    if (callbacktype != FMOD_CHANNELCONTROL_CALLBACK_END)
        return FMOD_OK;

    auto iter = channelSoundLookupTable.find(reinterpret_cast <FMOD_CHANNEL*> (channel));
    if (iter == channelSoundLookupTable.end())
        return FMOD_OK;

    auto* sound = iter->second.OPT_GET();
    channelSoundLookupTable.erase(iter);
    if (sound->isPersistent())
    {
        sound->play(true);
    }
    else
    {
        sound->close();
    }
    return FMOD_OK;
}

namespace FMODSound {
#define TRY(fname, ...) { FMOD_RESULT result = fname(__VA_ARGS__); if (result != FMOD_OK) throw Error(result, #fname); }
//##########################################################################################
void ChannelGroup::pauseAll()
{
    TRY(FMOD_ChannelGroup_SetPaused, group_, true);
}
//------------------------------------------------------------------------------------------
void ChannelGroup::unpauseAll()
{
    TRY(FMOD_ChannelGroup_SetPaused, group_, false);
}
//------------------------------------------------------------------------------------------
ChannelGroup::ChannelGroup(System* system, std::string const& name)
{
    TRY(FMOD_System_CreateChannelGroup, system->getSystem(), name.c_str(), &group_);
}
//------------------------------------------------------------------------------------------
ChannelGroup::~ChannelGroup()
{
    FMOD_ChannelGroup_Release(group_);
}
//------------------------------------------------------------------------------------------
FMOD_CHANNELGROUP* ChannelGroup::getGroup() const
{
    return group_;
}
//##########################################################################################
SpeakerMode::SpeakerMode(FMOD_SPEAKERMODE mode)
    : mode_(mode)
{
}
//------------------------------------------------------------------------------------------
SpeakerMode::operator FMOD_SPEAKERMODE() const
{
    return mode_;
}
//##########################################################################################
Error::Error(FMOD_RESULT fres)
    : Error(fres, {})
{

}
//------------------------------------------------------------------------------------------
Error::Error(FMOD_RESULT fres, std::string functionName)
    : result(fres)
    , functionName(std::move(functionName))
{

}
//------------------------------------------------------------------------------------------
const char* Error::what() const
{
    return "FMOD Exception";
}
//------------------------------------------------------------------------------------------
const char* Error::resultString() const
{
    return ResultEnum_strings[static_cast<size_t>(result)];
}
//##########################################################################################
SoundResource::SoundResource(System* system, FMOD_SOUND* handle, FMOD_CREATESOUNDEXINFO const& info)
    : system_(system)
    , handle_(handle)
    , alive_(true)
    , info_(info)
{

}
//------------------------------------------------------------------------------------------
SoundResource::~SoundResource()
{
    release();
}
//------------------------------------------------------------------------------------------
void SoundResource::release()
{
    if (!alive_)
        return;

    alive_ = false;
    FMOD_Sound_Release(handle_);
    system_->update();
}
//##########################################################################################
Sound::Sound(System* system, FMOD_SOUND* handle, bool persistent, const FMOD_CREATESOUNDEXINFO &info)
    : SoundResource(system, handle, info)
    , channel_(nullptr)
    , opened_(false)
    , persistent_(persistent)
{
}
//------------------------------------------------------------------------------------------
void Sound::close()
{
    opened_ = false;
}
//------------------------------------------------------------------------------------------
void Sound::play(bool paused, ChannelGroup* group)
{
    if (!alive_)
        return;

    FMOD_CHANNELGROUP* gp = nullptr;
    if (group != nullptr)
        gp = group->getGroup();

    TRY(FMOD_System_PlaySound, system_->getSystem(), handle_, gp, paused, &channel_);

    channelSoundLookupTable[channel_] = std::static_pointer_cast<FMODSound::Sound> (shared_from_this());

    FMOD_Channel_SetCallback(channel_, &channelCallback);

    system_->update();
    opened_ = true;
}
//------------------------------------------------------------------------------------------
bool Sound::isPaused() const
{
    if (!opened_)
        return false;

    FMOD_BOOL isP = false;
    TRY(FMOD_Channel_GetPaused, channel_, &isP);
    return isP;
}
//------------------------------------------------------------------------------------------
bool Sound::isPlaying() const
{
    if (!opened_)
        return false;

    FMOD_BOOL isP = false;
    TRY(FMOD_Channel_IsPlaying, channel_, &isP);
    return isP;
}
//------------------------------------------------------------------------------------------
bool Sound::isPersistent() const
{
    return persistent_;
}
//------------------------------------------------------------------------------------------
bool Sound::isOpened() const
{
    return opened_;
}
//------------------------------------------------------------------------------------------
void Sound::setVolume(float ratio)
{
    if (opened_)
    {
        TRY(FMOD_Channel_SetVolume, channel_, ratio);
        system_->update();
    }
}
//------------------------------------------------------------------------------------------
void Sound::pause()
{
    if (opened_ && !isPaused())
    {
        TRY(FMOD_Channel_SetPaused, channel_, true);
        system_->update();
    }
}
//------------------------------------------------------------------------------------------
void Sound::unpause()
{
    if (opened_ && isPaused())
    {
        TRY(FMOD_Channel_SetPaused, channel_, false);
        system_->update();
    }
}
//------------------------------------------------------------------------------------------
void Sound::stop()
{
    if (opened_)
    {
        TRY(FMOD_Channel_Stop, channel_);
    }
    system_->update();
}
//------------------------------------------------------------------------------------------
void Sound::setpause(bool paused)
{
    if (paused)
        pause();
    else
        unpause();
}
//------------------------------------------------------------------------------------------
float Sound::getVolume()
{
    if (!opened_)
        return 1.f;

    float volume;
    TRY(FMOD_Channel_GetVolume, channel_, &volume);
    return volume;
}
//------------------------------------------------------------------------------------------
std::string Sound::getName()
{
    char buffer[1024];
    FMOD_Sound_GetName(handle_, buffer, sizeof(buffer));
    return std::string(buffer);
}
//------------------------------------------------------------------------------------------
void Sound::saveWav(std::string const& fileName, bool prependHeader) const
{
    std::ofstream writer(fileName, std::ios_base::binary);
    if (!writer.good())
    {
        throw std::runtime_error("cannot open file");
    }
    else
        saveWav(writer, prependHeader);
}
//------------------------------------------------------------------------------------------
void Sound::setPosition(unsigned int position)
{
    if (opened_)
    {
        TRY(FMOD_Channel_SetPosition,
            channel_,
            position,
            FMOD_TIMEUNIT_MS
        );
    }
}
//------------------------------------------------------------------------------------------
unsigned int Sound::getPosition()
{
    if (opened_)
    {
        unsigned int position = 0;
        TRY(FMOD_Channel_GetPosition,
            channel_,
            &position,
            FMOD_TIMEUNIT_MS
        );
        return position;
    }
    return 0;
}
//------------------------------------------------------------------------------------------
unsigned int Sound::getLength()
{
    unsigned int length = 0;
    TRY(FMOD_Sound_GetLength,
        handle_,
        &length,
        FMOD_TIMEUNIT_MS
    );
    return length;
}
//------------------------------------------------------------------------------------------
void Sound::saveWav(std::ostream& stream, bool prependHeader) const
{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

#if defined(WIN32) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
    #define __PACKED                         /* dummy */
#else
    #define __PACKED __attribute__((packed)) /* gcc packed */
#endif
    auto& writer = stream;

    int channels, bits;
    float rate;
    unsigned int lenbytes;

    TRY(FMOD_Sound_GetFormat, handle_, 0, 0, &channels, &bits);
    TRY(FMOD_Sound_GetDefaults, handle_, &rate, 0);
    TRY(FMOD_Sound_GetLength, handle_, &lenbytes, FMOD_TIMEUNIT_PCMBYTES);

    {
        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack(1)
        #endif

        /*
            WAV Structures
        */
        typedef struct
        {
            signed char id[4];
            int 		size;
        } RiffChunk;

        struct
        {
            RiffChunk       chunk           __PACKED;
            unsigned short	wFormatTag      __PACKED;    /* format type  */
            unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
            unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
            unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
            unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
            unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
        } FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };

        struct
        {
            RiffChunk   chunk;
        } DataChunk = { {{'d','a','t','a'}, lenbytes } };

        struct
        {
            RiffChunk   chunk;
            signed char rifftype[4];
        } WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + lenbytes }, {'W','A','V','E'} };

        #if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
        #pragma pack()
        #endif

        /*
            Write out the WAV header.
        */
        if (prependHeader)
        {
            writer.write((char*)&WavHeader, sizeof(WavHeader));
            writer.write((char*)&FmtChunk, sizeof(FmtChunk));
            writer.write((char*)&DataChunk, sizeof(DataChunk));
        }

        unsigned int read = 0;
        char buffer[65536];
        FMOD_RESULT result = FMOD_OK;
        do {
            result = FMOD_Sound_ReadData(handle_, buffer, sizeof(buffer), &read);
            if (result != FMOD_OK)
                break;
            writer.write(buffer, read);
        } while (read == sizeof(buffer));
    }

#pragma GCC diagnostic pop
}
//##########################################################################################
SoundArchive::SoundArchive(System* system, FMOD_SOUND* handle, FMOD_CREATESOUNDEXINFO info, int subCount)
    : SoundResource(system, handle, info)
    , subCount_(subCount)
{
}
//------------------------------------------------------------------------------------------
int SoundArchive::getSubCount() const
{
    return subCount_;
}
//------------------------------------------------------------------------------------------
std::shared_ptr <Sound> SoundArchive::getSubSound(int index)
{
    if (index >= subCount_ || index < 0)
        throw std::out_of_range("subsound index invalid");

    FMOD_SOUND* sound;
    TRY(FMOD_Sound_GetSubSound, handle_, index, &sound);
    return std::shared_ptr <Sound> (new Sound(system_, sound, false, info_));
}
//##########################################################################################
System::System(FMOD_OUTPUTTYPE outputType, SpeakerMode speakerMode, int channelCount)
    : system_(nullptr)
    , version_(0u)
    , channelCount_(channelCount)
    , speakerMode_(speakerMode)
{
    TRY(FMOD_System_Create, &system_);
    TRY(FMOD_System_GetVersion, system_, &version_);
    //TRY(FMOD_System_SetSpeakerMode, system_, speakerMode);
    setOutput(outputType);
    TRY(FMOD_System_Init, system_, channelCount, FMOD_INIT_NORMAL, NULL);
    update();
}
//------------------------------------------------------------------------------------------
System::~System()
{
    update();
    FMOD_System_Close(system_);
}
//------------------------------------------------------------------------------------------
std::shared_ptr<ChannelGroup> System::createChannelGroup(std::string const& name)
{
    return std::shared_ptr<ChannelGroup>{new ChannelGroup(this, name)};
}
//------------------------------------------------------------------------------------------
std::shared_ptr <SoundResource> System::createSound(std::string const& fileName, int mode)
{
    FMOD_SOUND* sound;

    FMOD_CREATESOUNDEXINFO info;
    memset(&info, 0, sizeof(info));
    info.cbsize = sizeof(info);

    TRY(FMOD_System_CreateSound, system_, fileName.c_str(), mode, nullptr, &sound);
    update();

    auto snd = std::shared_ptr <SoundResource> (new Sound(this, sound, false, info));
    return snd;
}
//------------------------------------------------------------------------------------------
std::shared_ptr <SoundResource> System::createStream(std::string const& fileName, bool persistent, int mode)
{
    FMOD_SOUND* sound;

    FMOD_CREATESOUNDEXINFO info;
    memset(&info, 0, sizeof(info));
    info.cbsize = sizeof(info);

    TRY(FMOD_System_CreateStream, system_, fileName.c_str(), mode, nullptr, &sound);
    update();

    auto snd = std::shared_ptr <SoundResource> (new Sound(this, sound, persistent, info));
    return snd;
}
//------------------------------------------------------------------------------------------
std::shared_ptr<SoundResource> System::createStreamFromFSB(std::string const& fileName, int initialSubSound)
{
    FMOD_SOUND *sound;
    FMOD_CREATESOUNDEXINFO info;

    memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.initialsubsound = initialSubSound;

    TRY(FMOD_System_CreateSound, system_, fileName.c_str(), FMOD_CREATESTREAM, &info, &sound);
    update();

    int numsubsounds = 0;
    TRY(FMOD_Sound_GetNumSubSounds, sound, &numsubsounds);
    update();

    auto snd = std::shared_ptr <SoundResource> (new SoundArchive(this, sound, info, numsubsounds));
    return snd;
}
//------------------------------------------------------------------------------------------
FMOD_SYSTEM* System::getSystem()
{
    return system_;
}
//------------------------------------------------------------------------------------------
void System::update()
{
    FMOD_System_Update(system_);
}
//------------------------------------------------------------------------------------------
void System::setOutput(FMOD_OUTPUTTYPE type)
{
    TRY(FMOD_System_SetOutput, system_, type);
}
//------------------------------------------------------------------------------------------
FMOD_OUTPUTTYPE System::getOutput() const
{
    FMOD_OUTPUTTYPE type;
    TRY(FMOD_System_GetOutput, system_, &type);
    return type;
}
//##########################################################################################
FMOD_OUTPUTTYPE stringToOutputType(const std::string &typeName)
{
    if (typeName == "AUTODETECT")
        return (FMOD_OUTPUTTYPE_AUTODETECT);
    if (typeName == "UNKNOWN")
        return (FMOD_OUTPUTTYPE_UNKNOWN);
    if (typeName == "NOSOUND")
        return (FMOD_OUTPUTTYPE_NOSOUND);
    if (typeName == "WAVWRITER")
        return (FMOD_OUTPUTTYPE_WAVWRITER);
    if (typeName == "NOSOUND_NRT")
        return (FMOD_OUTPUTTYPE_NOSOUND_NRT);
    if (typeName == "WAVWRITER_NRT")
        return (FMOD_OUTPUTTYPE_WAVWRITER_NRT);
    if (typeName == "DSOUND")
        return (FMOD_OUTPUTTYPE_DSOUND);
    if (typeName == "WINMM")
        return (FMOD_OUTPUTTYPE_WINMM);
    if (typeName == "WASAPI")
        return (FMOD_OUTPUTTYPE_WASAPI);
    if (typeName == "ASIO")
        return (FMOD_OUTPUTTYPE_ASIO);
    if (typeName == "PULSEAUDIO")
        return (FMOD_OUTPUTTYPE_PULSEAUDIO);
    if (typeName == "ALSA")
        return (FMOD_OUTPUTTYPE_ALSA);
    if (typeName == "COREAUDIO")
        return (FMOD_OUTPUTTYPE_COREAUDIO);
    if (typeName == "XBOX360")
        return (FMOD_OUTPUTTYPE_XBOX360);
    if (typeName == "PS3")
        return (FMOD_OUTPUTTYPE_PS3);
    if (typeName == "AUDIOTRACK")
        return (FMOD_OUTPUTTYPE_AUDIOTRACK);
    if (typeName == "OPENSL")
        return (FMOD_OUTPUTTYPE_OPENSL);
    if (typeName == "WIIU")
        return (FMOD_OUTPUTTYPE_WIIU);
    if (typeName == "AUDIOOUT")
        return (FMOD_OUTPUTTYPE_AUDIOOUT);
    if (typeName == "MAX")
        return (FMOD_OUTPUTTYPE_MAX);
    return FMOD_OUTPUTTYPE_AUTODETECT;
}
//------------------------------------------------------------------------------------------
std::string outputTypeToString(FMOD_OUTPUTTYPE type)
{
    switch(type)
    {
        case(FMOD_OUTPUTTYPE_AUTODETECT):
            return "AUTODETECT";
        case(FMOD_OUTPUTTYPE_UNKNOWN):
            return "UNKNOWN";
        case(FMOD_OUTPUTTYPE_NOSOUND):
            return "NOSOUND";
        case(FMOD_OUTPUTTYPE_WAVWRITER):
            return "WAVWRITER";
        case(FMOD_OUTPUTTYPE_NOSOUND_NRT):
            return "NOSOUND_NRT";
        case(FMOD_OUTPUTTYPE_WAVWRITER_NRT):
            return "WAVWRITER_NRT";
        case(FMOD_OUTPUTTYPE_DSOUND):
            return "DSOUND";
        case(FMOD_OUTPUTTYPE_WINMM):
            return "WINMM";
        case(FMOD_OUTPUTTYPE_WASAPI):
            return "WASAPI";
        case(FMOD_OUTPUTTYPE_ASIO):
            return "ASIO";
        case(FMOD_OUTPUTTYPE_PULSEAUDIO):
            return "PULSEAUDIO";
        case(FMOD_OUTPUTTYPE_ALSA):
            return "ALSA";
        case(FMOD_OUTPUTTYPE_COREAUDIO):
            return "COREAUDIO";
        case(FMOD_OUTPUTTYPE_XBOX360):
            return "XBOX360";
        case(FMOD_OUTPUTTYPE_PS3):
            return "PS3";
        case(FMOD_OUTPUTTYPE_AUDIOTRACK):
            return "AUDIOTRACK";
        case(FMOD_OUTPUTTYPE_OPENSL):
            return "OPENSL";
        case(FMOD_OUTPUTTYPE_WIIU):
            return "WIIU";
        case(FMOD_OUTPUTTYPE_AUDIOOUT):
            return "AUDIOOUT";
        case(FMOD_OUTPUTTYPE_MAX):
            return "MAX";
        default:
            return "UNKNOWN";
    }
}
//##########################################################################################
MediaPlayer::MediaPlayer(System* fsys, float volume)
    : curSound_()
    , list_()
    , position_()
    , volume_(volume)
    , fsys_(fsys)
{

}
//------------------------------------------------------------------------------------------
MediaPlayer::~MediaPlayer()
{
    if (curSound_ && curSound_.OPT_GET()->isOpened())
        curSound_.OPT_GET()->stop();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::clear()
{
    position_.reset();
    //if (curSound_ && curSound_.OPT_GET()->isOpened() && curSound_.OPT_GET()->isPlaying())
    //    curSound_.OPT_GET()->stop();
    //curSound_.reset();
    list_.clear();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::add(std::string const& sound)
{
    list_.push_back(sound);
}
//------------------------------------------------------------------------------------------
void MediaPlayer::play()
{
    if (curSound_)
    {
        auto playLambda = [this]()
        {
                curSound_.OPT_GET()->play(true);
                curSound_.OPT_GET()->setVolume(volume_);
                curSound_.OPT_GET()->unpause();
                currentPlayingPosition_ = position_.OPT_GET();
        };

        // if position got invalidated always reload track
        if (!position_)
        {
            stop();
            curSound_.reset();
            play();
            return;
        }
            if (currentPlayingPosition_ && position_.OPT_GET() != currentPlayingPosition_.OPT_GET())
        {
                if (curSound_.OPT_GET()->isOpened())
                stop();
                curSound_ = std::static_pointer_cast <FMODSound::Sound> (fsys_->createStream(list_[position_.OPT_GET()]));
                playLambda();
        }
            else if (curSound_.OPT_GET()->isOpened() && curSound_.OPT_GET()->isPaused())

        {
                curSound_.OPT_GET()->unpause();
        }
        else
        {
            playLambda();
        }
    }
    else
    {
        if (list_.empty())
            return;

        if (!position_)
            position_ = 0;

        curSound_ = std::static_pointer_cast <FMODSound::Sound> (fsys_->createStream(list_[position_.OPT_GET()]));
        play();
    }
}
//------------------------------------------------------------------------------------------
void MediaPlayer::pause()
{
    if (curSound_ && curSound_.OPT_GET()->isOpened() && curSound_.OPT_GET()->isPlaying())
        curSound_.OPT_GET()->pause();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::next(bool rollOver)
{
    if (!position_)
    {
        if (!list_.empty())
            position_ = 0;
        else
            return;
    }
    else if (position_.OPT_GET() + 1 < list_.size())
    {
        position_.OPT_GET()++;
    }
    else if (rollOver && !list_.empty())
    {
        position_.OPT_GET() = 0;
    }

    play();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::previous(bool rollOver)
{
    if (!position_)
    {
        if (list_.size() > 0)
            position_ = list_.size() - 1;
        else
            return;
    }
    else if (position_.OPT_GET() > 0 && !list_.empty())
    {
        position_.OPT_GET()--;
    }
    else if (rollOver && !list_.empty())
    {
        position_.OPT_GET() = list_.size() - 1;
    }

    play();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::stop()
{
    if (curSound_ && curSound_.OPT_GET()->isOpened() && (curSound_.OPT_GET()->isPlaying() || curSound_.OPT_GET()->isPaused()))
    {
        curSound_.OPT_GET()->stop();
        curSound_.reset();
    }
}
//------------------------------------------------------------------------------------------
void MediaPlayer::togglePause()
{
    // if the position got invalidated: try relod in any case:
    if (!position_) {
        stop();
        curSound_.reset();
        play();
        return;
    }

    // otherwise:
    if (position_ && currentPlayingPosition_ && position_.OPT_GET() != currentPlayingPosition_.OPT_GET())
    {
        play();
    }
    else if (curSound_ && curSound_.OPT_GET()->isOpened())
    {
        if (curSound_.OPT_GET()->isPaused())
            curSound_.OPT_GET()->unpause();
        else if (curSound_.OPT_GET()->isPlaying())
            curSound_.OPT_GET()->pause();
    }
    else
        play();
}
//------------------------------------------------------------------------------------------
void MediaPlayer::setVolume(float volume)
{
    volume_ = volume;
    if (curSound_ && curSound_.OPT_GET()->isOpened())
    {
        if (curSound_.OPT_GET()->isPlaying())
        {
            curSound_.OPT_GET()->pause();
            curSound_.OPT_GET()->setVolume(volume_);
            curSound_.OPT_GET()->unpause();
        }
        else if (curSound_.OPT_GET()->isPaused())
        {
            curSound_.OPT_GET()->setVolume(volume_);
            curSound_.OPT_GET()->unpause();
        }
    }
}
//------------------------------------------------------------------------------------------
float MediaPlayer::getVolume() const
{
    return volume_;
}
//------------------------------------------------------------------------------------------
void MediaPlayer::setIndex(std::size_t position)
{
    if (position_ < list_.size())
    {
        position_ = position;
    }
}
//------------------------------------------------------------------------------------------
boost::optional<std::size_t> MediaPlayer::getIndex() const
{
    return position_;
}
//------------------------------------------------------------------------------------------
void MediaPlayer::setPosition(unsigned int position)
{
    if (curSound_)
        curSound_.OPT_GET()->setPosition(position);
}
//------------------------------------------------------------------------------------------
unsigned int MediaPlayer::getPosition() const
{
    if (curSound_)
        return curSound_.OPT_GET()->getPosition();
    return 0;
}
//------------------------------------------------------------------------------------------
unsigned int MediaPlayer::getLength() const
{
    if (curSound_)
        return curSound_.OPT_GET()->getLength();
    return 0;
}
//------------------------------------------------------------------------------------------
bool MediaPlayer::isPaused() const
{
    return curSound_ && curSound_.OPT_GET()->isOpened() && curSound_.OPT_GET()->isPaused();
}
//------------------------------------------------------------------------------------------
bool MediaPlayer::isPlaying() const
{
    return curSound_ && curSound_.OPT_GET()->isOpened() && curSound_.OPT_GET()->isPlaying();
}
//------------------------------------------------------------------------------------------

} // namespace FMODSound

#undef TRY
