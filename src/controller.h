#ifndef CONTROLLER_H
#define CONTROLLER_H


#include <string>
#include <vector>
#include <map>


class Controller
{
  public:
    Controller() : name("unnamed") {}
    virtual ~Controller() {}
    virtual void Sustain(float dt) = 0;
    virtual float GetAxisValue(int id) const = 0;
    virtual bool  GetButtonValue(int id) const = 0;
    virtual float GetSteer(void) const = 0;
    virtual float GetAccel(void) const = 0;
    virtual float GetBrake(void) const = 0;
    virtual bool  GetEBrake(void) const = 0;
    virtual bool  GetAction(void) const = 0;
  protected:
    std::string name;
};


class ControllerPad : public Controller
{
  public:
    ControllerPad(const std::string &devfile);
    virtual ~ControllerPad();
    virtual void Sustain(float dt);
    virtual float GetAxisValue(int id) const;
    virtual bool  GetButtonValue(int id) const;
    virtual bool  GetButtonChanged(int id); // destructive read
    virtual bool  HasAnyButtonChanged(bool down); // destructive read
    virtual bool  IsWorking(void) const { return opened; }
    virtual float GetSteer(void) const;
    virtual float GetAccel(void) const;
    virtual float GetBrake(void) const;
    virtual bool  GetEBrake(void) const;
    virtual bool  GetAction(void) const;
    virtual void  Introspect(void);
  protected:
#ifdef WIN32
    int id;
#endif
    bool opened;
    int fd;
    unsigned char axiscount, buttoncount;
    std::vector<short> AxisValues;
    std::vector<bool>  ButtonValues;
    std::vector<bool>  ButtonChanged;
    float DeadMargin;
    int *joydb_entry;
    int steer_axis;
    int brake_axis;
    int brake_button;
    int ebrake_button;
    int action_button;
    int accel_axis;
    int accel_button;
    int brake_polarity;
    int accel_polarity;
};


class ControllerKey : public Controller
{
  public:
    ControllerKey(const std::string &map_fname);
    ~ControllerKey(void);
    virtual void Sustain(float dt);
    virtual float GetAxisValue(int id) const;
    virtual bool  GetButtonValue(int id) const;
    virtual float GetSteer(void) const { return GetAxisValue(0); }
    virtual float GetAccel(void) const { return GetAxisValue(1); }
    virtual float GetBrake(void) const { return GetAxisValue(2); }
    virtual bool  GetEBrake(void) const { return key_ebrake; }
    virtual bool  GetAction(void) const { return key_action; }
    void FeedKey(unsigned char key, bool down);
    void FeedAction(const std::string &action, bool down);
  protected:
    float axes[3];
    bool key_steerl, key_steerr, key_accel, key_brake, key_action, key_ebrake;
    std::map<unsigned char, std::string> mapping;
};

#endif

