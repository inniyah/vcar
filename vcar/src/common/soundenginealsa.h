
#ifndef SOUNDENGINE_ALSA_H
#define SOUNDENGINE_ALSA_H

#include <string>

#include <alsa/asoundlib.h>

class SoundClip;
class SoundFeed;
class SoundFeedSimple;
class SoundFeedComplex;
class SoundFeedModulated;
class SoundFeedEngine;

class SoundEngineAlsa
{
  public:
    SoundEngineAlsa(int framelag=4400);
    ~SoundEngineAlsa();
    void Play(const std::string &fname, int delay=0, bool looping=false);
    void StopPlay(void);
    float Sustain(void);
    void SetMode(const std::string &modename);
    void SetModulation(float freq, float amp);
    void SetLowPass(float f);
    bool opened;
  protected:
    SoundFeedSimple    *simplefeed;
    SoundFeedComplex   *complexfeed;
    SoundFeedModulated *modulatedfeed;
    SoundFeedEngine    *enginefeed;
    SoundFeed          *activefeed;
    snd_pcm_t *handle;
    snd_pcm_uframes_t periodsz; // in frames
    snd_pcm_uframes_t buffersz; // in frames
    int batchsize; // in periods
    int framelag; // in frames
    float lpfilter;
};

#endif
