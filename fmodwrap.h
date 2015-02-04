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
        System(FMOD_OUTPUTTYPE outputType = FMOD_OUTPUTTYPE_AUTODETECT, SpeakerMode speakerMode = FMOD_SPEAKERMODE_DEFAULT, int channelCount = 32);
        ~System();

        // mode can be: FMOD_DEFAULT, FMOD_SOFTWARE, FMOD_CREATECOMPRESSEDSAMPLE
        std::shared_ptr<ChannelGroup> createChannelGroup(std::string const& name);
        std::shared_ptr<SoundResource> createSound(std::string const& fileName, int mode = FMOD_DEFAULT);
        std::shared_ptr<SoundResource> createStream(std::string const& fileName, bool persistent = false, int mode = FMOD_DEFAULT);
        std::shared_ptr<SoundResource> createStreamFromFSB(std::string const& fileName, int initialSubSound = 0);

        FMOD_SYSTEM* getSystem();
        void update();
        void setOutput(FMOD_OUTPUTTYPE type);
        FMOD_OUTPUTTYPE getOutput() const;
    };

//##########################################################################################

    FMOD_OUTPUTTYPE stringToOutputType(std::string const& typeName);
    std::string outputTypeToString(FMOD_OUTPUTTYPE type);

//##########################################################################################

    class MediaPlayer
    {
    private:
        boost::optional <std::shared_ptr <Sound>> curSound_;
        std::vector <std::string> list_;
        boost::optional <std::size_t> position_;
        boost::optional <std::size_t> currentPlayingPosition_;
        float volume_;
        System* fsys_;

    public:
        /**
         * @brief Creates a MediaPlayer instance, which needs a sound system that does all the playing and loading.
         * @param fsys A FMOD System used internally for loading and playing tracks etc.
         * @param volume A standard volume that is first used for playback.
         */
        MediaPlayer(System* fsys, float volume = 0.5f);
        ~MediaPlayer();

        /**
         * @brief Clears the track list.
         */
        void clear();

        /**
         * @brief Add a file to the track list.
         * @param file A file of an accepted format (see FMOD documentation).
         */
        void add(std::string const& file);

        /**
         * @brief Plays the sound if its loaded, unpauses the track if it is paused and reloads the track whenever the index changed.
         */
        void play();

        /**
         * @brief Pauses the track. Does not unpause the track when called twice.
         */
        void pause();

        /**
         * @brief Changes the position index and automatically plays the next track (and therefore stops the last).
         * @param rollOver When the current track is the last one in the list, go back to the beginning if set to true.
         */
        void next(bool rollOver = false);

        /**
         * @brief Changes the position index and automatically plays the previous track (and therefore stops the last).
         * @param rollOver When the current track is the first one in the list, go back to the end if set to true.
         */
        void previous(bool rollOver = false);

        /**
         * @brief Stops the track completely. The next time play is called the currently selected track (by index) is loaded anew.
         * This also means that the current track position is lost.
         */
        void stop();

        /**
         * @brief Toggles the pause state of the track (pauses / unpauses accordingly).
         */
        void togglePause();

        /**
         * @brief Sets the volume of the playback.
         * @param volume A value from 0.f (no sound at all) to 1.f (max volume).
         */
        void setVolume(float volume);

        /**
         * @brief Returns the current volume.
         * @return Returns a value from 0.f (no sound at all) to 1.f (max volume).
         */
        float getVolume() const;

        /**
         * @brief Sets the current track list index (must not exceed list size). If the position exceeds the size of the list
         * it ignores the call (does not throw).
         *
         * @param position A position in the tracklist.
         */
        void setIndex(std::size_t position);

        /**
         * @brief Returns the currently selected track index.
         * @return Returns the track index.
         */
        boost::optional <std::size_t> getIndex() const;

        /**
         * @brief Sets the track position in milliseconds
         * @param position A position on the track in milliseconds.
         */
        void setPosition(unsigned int position);

        /**
         * @brief getPosition Returns the current track position in milliseconds.
         * @return Returns the current track position in millisecond.
         */
        unsigned int getPosition() const;

        /**
         * @brief getLength Returns the track length in milliseconds.
         * @return Returns the track length in milliseconds.
         */
        unsigned int getLength() const;

        /**
         * @brief isPaused returns whether a track is opened and paused.
         * @return Returns true if the current track is paused, if one is opened.
         */
        bool isPaused() const;

        /**
         * @brief isPlaying returns whether a track is opened and playing.
         * @return Returns true if the current track is playing, if one is opened.
         */
        bool isPlaying() const;
    };
}

#pragma GCC diagnostic pop

#endif // SYSTEM_H
