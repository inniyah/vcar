#ifndef SOUNDFEED_H
#define SOUNDFEED_H

#include <cstring>
#include "soundclip.h"


// Abstract baseclass for all soundfeed objects
class SoundFeed
{
  public:

    SoundFeed(int psz) : periodsz(psz) {}
    virtual ~SoundFeed() {}

    virtual void Paste(const std::string &fname, int delay, bool looping=false) = 0;
    virtual short *Get(int &numperiods) = 0;
    virtual float FractionDone(int delay) { return 0.0; }

  protected:

    int periodsz;	// in frames
};


// A soundfeed that plays a single soundclip, to be used for musicscore e.g.
class SoundFeedSimple : public SoundFeed
{
  public:

    SoundFeedSimple(int psz) :
      SoundFeed(psz),
      clip(0),
      currentperiod(0)
    {
    }

    virtual ~SoundFeedSimple()
    {
      clip = 0;
    }

    virtual void Paste(const std::string &fname, int delay, bool looping=false)
    {
      assert(!delay);
      clip = SoundClip::Get(fname);
      currentperiod = 0;
    }

    virtual short *Get(int &numperiods)
    {
      if (clip)
      {
        int maxperiod = clip->framecnt/periodsz;
        if (currentperiod < maxperiod)
        {
          int togo = maxperiod - currentperiod;
          if (numperiods > togo)
            numperiods = togo;
          short *retval = clip->data + currentperiod * 2 * periodsz;
          currentperiod += numperiods;
          return retval;
        }
      }
      numperiods = 0;
      return 0;
    }

    virtual float FractionDone(int delay)
    {
      // delay is the nr of frames still in the driver soundbuffer.
      if (!clip) return 0.0;
      int framesdone = currentperiod*periodsz - delay;
      return framesdone / (float) ((clip->framecnt/periodsz)*periodsz);
    }

  protected:

    SoundClip *clip;
    int currentperiod;
};


// A soundfeed that mixes multiple channels, to be used in-game
class SoundFeedComplex : public SoundFeed
{
  class ModulatedChannel
  {
    public:
      ModulatedChannel(int psz) :
        periodsz(psz),
        clip(0),
        sz(0),
        idx(0.0),
        freq(1.0),
        ampl(1.0)
      {
        scratch = new short[100000];
      }
      virtual ~ModulatedChannel()
      {
        delete scratch;
      }
      virtual void Paste(const std::string &fname, int delay, bool looping=false)
      {
        // get the mono soundclip.
        clip = SoundClip::Get(fname, 1);
        sz = clip->framecnt;
        idx = 0.0;
      }
      virtual short *Get(int &numframes)
      {
        if (!clip)
        {
          numframes=0;
          return 0;
        }
        short *writer=scratch;
        for (int i=0; i<numframes; i++)
        {
          int v = (int) (ampl * clip->data[(int)idx]);
          if (v>32767)  v=32767;
          if (v<-32768) v=-32768;
          writer[0] = v;
          writer[1] = v;
          writer += 2;
          idx += freq;
          if (idx >= sz)
            idx -= sz;
        }
        return scratch;
      }
      void SetModulation(float f, float a)
      {
        freq = f;
        ampl = a;
      }
    protected:
      int periodsz;
      SoundClip *clip;
      int sz;
      float idx;
      short *scratch;
      float freq;
      float ampl;
  };

  class Channel
  {
    public:
      Channel() : data(0), sz(0), sent(0), is_looping(false) {}
      virtual ~Channel() { delete data; data=0; }
      virtual void Paste(const std::string &fname, int delay, bool looping)
      {
        // delay is in nr of frames
        SoundClip *clip = SoundClip::Get(fname);
        is_looping = looping;
        if (is_looping) assert(!delay);
        sz = clip->framecnt + delay;
        delete data;
        if (is_looping)
        {
          data  = new short [sz*2*2];
          memcpy(data,      clip->data, clip->framecnt*2*sizeof(short));
          memcpy(data+sz*2, clip->data, clip->framecnt*2*sizeof(short));
        }
        else
        {
          data  = new short [sz*2];
          memset(data, 0, delay*2*sizeof(short));
          memcpy(data+2*delay, clip->data, clip->framecnt*2*sizeof(short));
        }
        sent=0;
      }
      virtual short *Get(int &numframes)
      {
        if (!sz || (sent==sz && !is_looping))
        {
          numframes=0;
          return 0;
        }

        short *retval=data+2*sent;
        if (is_looping)
        {
          retval = data + 2*sent;
          sent += numframes;
          if (sent > sz) sent -= sz;
        }
        else
        {
          int togo = sz - sent;
          if (numframes > togo)
            numframes = togo;
          sent += numframes;
        }
        return retval;
      }
      bool IsIdle(void) const 
      {
        if (is_looping) return false;
        return sent==sz; 
      }
      float GetCompletionFactor(void) const
      {
        if (is_looping) return 0.0;
        if (!sz) return 1.0;
        return sent / (float) sz;
      }
    protected:
      short *data;
      int    sz;	// in nr of frames
      int    sent;	// in nr of frames
      bool   is_looping;
  };

  public:

    SoundFeedComplex(int psz) : 
      SoundFeed(psz),
      modulated_channel(psz)
    {
      scratch32 = new int   [100000];
      scratch16 = new short [100000];
    }
    ~SoundFeedComplex()
    {
      delete scratch32;
      delete scratch16;
    }
    short *Get(int &numperiods)
    {
      int numframes = numperiods * periodsz;
      int framecounts[NUMCHANNELS+1];
      short *subdata[NUMCHANNELS+1];
      int total=0;
      int i;
      for (i=0; i<NUMCHANNELS; i++)
      {
        framecounts[i] = numframes;
        subdata[i] = channels[i].Get(framecounts[i]);
        total += framecounts[i];
      }
      framecounts[NUMCHANNELS]=numframes;
      subdata[NUMCHANNELS] = modulated_channel.Get(framecounts[NUMCHANNELS]);
      total += framecounts[NUMCHANNELS];
      if (!total)
      {
        // All channels are silent
        numperiods = 0;
        return 0;
      }
      int periodcounts[NUMCHANNELS+1];
      int max=0;
      for (i=0; i<NUMCHANNELS+1; i++)
      {
        periodcounts[i] = (framecounts[i] + (periodsz-1)) / periodsz;
        if (periodcounts[i] > max)
          max=periodcounts[i];
      }
      numperiods = max;
      int sz = numperiods * periodsz;
      memset(scratch32, 0, sz*2*sizeof(int));
      for (i=0; i<NUMCHANNELS; i++)
      {
        short *data = subdata[i];
        for (int j=0; j<2*framecounts[i]; j++)
          scratch32[j] += data[j];
      }
      for (int j=0; j<2*framecounts[NUMCHANNELS]; j++)
        scratch32[j] += subdata[NUMCHANNELS][j];

      for (int j=0; j<2*sz; j++)
        scratch16[j] = (scratch32[j]>32767)?32767:(scratch32[j]<-32768)?-32768:scratch32[j];
      return scratch16;
    }
    int GetIdleChannel(void) const
    {
      float maxv = 0.0;
      int maxi = -1;
      for (int j=0; j<NUMCHANNELS; j++)
      {
        float v = channels[j].GetCompletionFactor();
        if (v>maxv)
        {
          maxv = v;
          maxi = j;
        }
      }
      return maxi;
    }
    void Paste(const std::string &fname, int delay, bool looping=false)
    {
      if (looping)
      {
        modulated_channel.Paste(fname, delay, looping);
        return;
      }
      int channr = GetIdleChannel();
      if (channr == -1) return;
      channels[channr].Paste(fname, delay, looping);
    }
    void SetModulation(float f, float a)
    {
      modulated_channel.SetModulation(f,a);
    }

  protected:

    static const int NUMCHANNELS=7;
    Channel channels[NUMCHANNELS];
    ModulatedChannel modulated_channel;
    int   *scratch32;
    short *scratch16;
    int    modulation_numerator;
};


class SoundFeedModulated : public SoundFeed
{
  public:
    SoundFeedModulated(int psz) :
      SoundFeed(psz),
      clip(0),
      sz(0),
      idx(0.0),
      freq(1.0),
      ampl(1.0)
    {
      scratch = new short[100000];
    }
    virtual ~SoundFeedModulated()
    {
      delete scratch;
    }
    virtual void Paste(const std::string &fname, int delay, bool looping=false)
    {
      // get the mono soundclip.
      clip = SoundClip::Get(fname, 1);
      sz = clip->framecnt;
      idx = 0.0;
    }
    virtual short *Get(int &numperiods)
    {
      if (!clip)
      {
        numperiods=0;
        return 0;
      }
      int numframes = numperiods*periodsz;
      short *writer=scratch;
      for (int i=0; i<numframes; i++)
      {
        int v = (int) (ampl * clip->data[(int)idx]);
        if (v>32767)  v=32767;
        if (v<-32768) v=-32768;
        writer[0] = v;
        writer[1] = v;
        writer += 2;
        idx += freq;
        if (idx >= sz)
          idx -= sz;
      }
      return scratch;
    }
    void SetModulation(float f, float a)
    {
      freq = f;
      ampl = a;
    }
  protected:
    SoundClip *clip;
    int sz;
    float idx;
    short *scratch;
    float freq;
    float ampl;
};


class SoundFeedEngine : public SoundFeed
{
  public:
    SoundFeedEngine(int psz) :
      SoundFeed(psz),
      freq(1.0),
      ampl(1.0),
      sz(0),
      rps(1.0),
      cycletime((int)((1/rps) * 44100)),
      clip(0)
    {
      scratch = new short[100000];
      SetModulation(1,1);
    }
    ~SoundFeedEngine()
    {
      delete scratch;
    }
    virtual void Paste(const std::string &fname, int delay, bool looping=false)
    {
      // get the mono soundclip.
      clip = SoundClip::Get(fname, 1);
      sz = clip->framecnt;
      float step = cycletime/NUMCYLS;
      for (int i=0; i<NUMCYLS; i++)
      {
        framecounters[i]=sz;
        restartdelays[i]=step*i;
      }
    }
    virtual short *Get(int &numperiods)
    {
      if (!clip)
      {
        numperiods=0;
        return 0;
      }
      int numframes = numperiods*periodsz;
      short *writer=scratch;
      for (int i=0; i<numframes; i++)
      {
        int j;
        for (j=0; j<NUMCYLS; j++)
        {
          if (restartdelays[j]<=0.0)
          {
            restartdelays[j] += cycletime;
            framecounters[j] = 0;
          }
          restartdelays[j] -= freq;
        }
        int vals[2]={0,0};
        for (j=0; j<NUMCYLS; j++)
        {
          if (framecounters[j] < sz)
          {
            vals[j&1] += clip->data[framecounters[j]];
            framecounters[j] += 1;
          }
        }
        vals[0] = (int) (vals[0] * ampl);
        vals[1] = (int) (vals[1] * ampl);
        if (vals[0]>32767) vals[0]=32767;
        if (vals[0]<-32768) vals[0]=-32768;
        if (vals[1]>32767) vals[1]=32767;
        if (vals[1]<-32768) vals[1]=-32768;
        *writer++ = (short) vals[0];
        *writer++ = (short) vals[1];
      }
      return scratch;
    }
    void SetModulation(float f, float a)
    {
      freq = f;
      ampl = a;
    }
  protected:
    static const int NUMCYLS=8;
    int framecounters[NUMCYLS];
    float restartdelays[NUMCYLS];
    float freq;
    float ampl;
    int sz;
    float rps;		// revs per second
    int cycletime; 	// in nr of frames
    SoundClip *clip;
    short *scratch;
};


#endif

