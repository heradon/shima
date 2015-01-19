#ifndef SYSTEM_H
#define SYSTEM_H

#include <fmod.h>
#include <string>
#include <memory>
#include <vector>
#include <boost/optional.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

namespace FMODSound
{
    class System;
    class Sound;

//##########################################################################################

    class ChannelGroup
    {
        friend class System;

    private:
        FMOD_CHANNELGROUP* group_;

    public:
        void pauseAll();
        void unpauseAll();

        ChannelGroup(System* system, std::string const& name);
        ~ChannelGroup();

        FMOD_CHANNELGROUP* getGroup() const;
    };

//##########################################################################################

    class SpeakerMode
    {
    private:
        FMOD_SPEAKERMODE mode_;

    public:
        SpeakerMode(FMOD_SPEAKERMODE mode = FMOD_SPEAKERMODE_STEREO);
        operator FMOD_SPEAKERMODE() const;
    };

//##########################################################################################

    struct Error
    {
        FMOD_RESULT result;
        std::string functionName;

        const char* resultString() const;

        Error(FMOD_RESULT fres);
        Error(FMOD_RESULT fres, std::string functionName);

        const char* what() const;
    };

//##########################################################################################

    class SoundResource : public std::enable_shared_from_this <SoundResource>
    {
        friend class System;

    protected:
        System* system_;
        FMOD_SOUND* handle_;
        bool alive_;
        FMOD_CREATESOUNDEXINFO info_;

    protected:
        SoundResource(System* system, FMOD_SOUND* handle, FMOD_CREATESOUNDEXINFO const& info);

    public:
        void release();
        virtual ~SoundResource();
    };

//##########################################################################################

    class Sound : public SoundResource
    {
    private:
        FMOD_CHANNEL* channel_;
        bool opened_;
        bool persistent_;

    public:
        /**
         * Creates a sound object.
         *
         * @param system The fmod soundsystem to run on.
         * @param handle A sound handle created by the system.
         * @param persistent The file will reload itself on sound end if set to true.
         */
        Sound(System* system, FMOD_SOUND* handle, bool persistent, FMOD_CREATESOUNDEXINFO const& info);

        void play(bool paused = false, ChannelGroup* group = nullptr);
        bool isPaused() const;
        bool isPlaying() const;
        bool isPersistent() const;
        bool isOpened() const;
        void setVolume(float ratio);
        std::string getName();
        void pause();
        void unpause();
        void stop();
        void setpause(bool paused);
        float getVolume();
        void saveWav(std::string const& fileName, bool prependHeader = true) const;
        void saveWav(std::ostream& stream, bool prependHeader = false) const;
        void setPosition(unsigned int position);
        unsigned int getPosition();
        unsigned int getLength();

        // do not use manually:
        void close();
    };

//##########################################################################################

    class SoundArchive : public SoundResource
    {
        friend System;

    private:
        int subCount_;

    private:
        SoundArchive(System* system, FMOD_SOUND* handle, FMOD_CREATESOUNDEXINFO info, int subCount);

    public:
        int getSubCount() const;
        std::shared_ptr <Sound> getSubSound(int index);
    };

//##########################################################################################

    class System
    {
    private:
        FMOD_SYSTEM* system_;
        unsigned int version_;
        int channelCount_;
        SpeakerMode speakerMode_;

    private:

    public:
        System(SpeakerMode speakerMode = FMOD_SPEAKERMODE_DEFAULT, int channelCount = 32);
        ~System();

        // mode can be: FMOD_DEFAULT, FMOD_SOFTWARE, FMOD_CREATECOMPRESSEDSAMPLE
        std::shared_ptr<ChannelGroup> createChannelGroup(std::string const& name);
        std::shared_ptr<SoundResource> createSound(std::string const& fileName, int mode = FMOD_DEFAULT);
        std::shared_ptr<SoundResource> createStream(std::string const& fileName, bool persistent = false, int mode = FMOD_DEFAULT);
        std::shared_ptr<SoundResource> createStreamFromFSB(std::string const& fileName, int initialSubSound = 0);

        FMOD_SYSTEM* getSystem();
        void update();
    };

//##########################################################################################

    class MediaPlayer
    {
    private:
        boost::optional <std::shared_ptr <Sound>> curSound_;
        std::vector <std::string> list_;
        boost::optional <std::size_t> position_;
        float volume_;
        System* fsys_;

    public:
        MediaPlayer(System* fsys, float volume = 0.5f);
        ~MediaPlayer();

        void clear();
        void add(std::string const& file);
        void play();
        void pause();
        void next(bool rollOver = false);
        void previous(bool rollOver = false);
        void stop();
        void togglePause();

        void setVolume(float volume);
        float getVolume() const;

        void setIndex(std::size_t position);
        std::size_t getIndex() const;

        void setPosition(unsigned int position);
        unsigned int getPosition() const;
        unsigned int getLength() const;
    };
}

#pragma GCC diagnostic pop

#endif // SYSTEM_H
