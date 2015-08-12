#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H

#include <string>
#include <cstring>
#include <map>


class SoundClip;
typedef std::map<std::string, SoundClip *> clipmap_t;

class SoundClip
{
  public :
    SoundClip(const std::string &nm, int numc=2) :
      name(nm),
      framecnt(0),
      data(0),
      numchannels(numc)
    {
      struct stat s;
      int rv=stat(nm.c_str(), &s);
      if (rv)
        perror("stat() failed");
      assert(!rv);
      int hdrsz=0;
      if (strstr(nm.c_str(),".wav"))
        hdrsz=44;
      framecnt = (s.st_size-hdrsz) / (2*numc);
      FILE *f=fopen(nm.c_str(),"rb");
      assert(f);
      if (hdrsz)
      {
        rv=fseek(f, hdrsz, SEEK_CUR);
        assert(!rv);
      }
      data = new short[numchannels*framecnt];
      int datasz = framecnt*2*numchannels;
      rv=fread(data, datasz, 1, f);
      assert(rv==1);
      fclose(f);
      if (bigendian)
      {
        unsigned char tmp;
        for (int i=0; i<framecnt*numchannels; i++)
        {
          unsigned char *p = (unsigned char *)(data+i);
          tmp=p[0]; p[0]=p[1]; p[1]=tmp;
        }
      }
    }
    ~SoundClip()
    {
      delete [] data;
    }
    static SoundClip *Get(const std::string &name, int numc=2)
    {
      clipmap_t::const_iterator ip = clips.find(name);
      if (ip == clips.end())
      {
        clips[name] = new SoundClip(name, numc);
        return clips[name];
      }
      else
      {
        return (*ip).second;
      }
    }

    std::string name;
    int framecnt;
    short *data;
    int numchannels;
    static bool bigendian;

  protected:
    static clipmap_t clips;
};

#endif
