#ifndef GAME_CLIENT_AUDIO_SCRIPTEDSOUNDENGINE_HPP
#define GAME_CLIENT_AUDIO_SCRIPTEDSOUNDENGINE_HPP

#include "game/client/audio/ConsoleSoundEngine.hpp"

class ScriptedSoundEngine : public ConsoleSoundEngine
{
public:
    virtual void tick(boost::shared_ptr<Mob>* listeners, float partialTicks);
    virtual void destroy();
    virtual void shutdown();
    virtual void play(const SoundEvent* sound, float x, float y, float z, float volume, float pitch,
                      SoundInstance* instance);
    virtual void playStreaming(const SoundEvent* sound, float x, float y, float z, float volume, float pitch,
                               bool checkPosition, bool waitForMusicStartEvent, eAudioDelayType delayType);
    virtual void playUI(const SoundEvent* sound, float volume, float pitch);
    virtual void init(Options* options);
    virtual void add(const std::wstring& name, File* file);
    virtual void addMusic(const std::wstring& name, File* file);
    virtual void addStreaming(const std::wstring& name, File* file);
    virtual void playMusicTick();
    virtual void SetStreamingSounds(int overworldFirst, int overworldLast, int netherFirst, int netherLast,
                                    int endFirst, int endLast, int creativeFirst, int creativeLast, int menuFirst,
                                    int menuLast, int battleFirst, int battleLast, int tumbleFirst, int tumbleLast,
                                    int glideFirst, int glideLast, int buildFirst, int buildLast, int firstCDTrack);
    virtual void FlagMusicEventStart();
    virtual void updateMusicVolume(float volume);
    virtual void updateSoundEffectVolume(float volume);
};

#endif
