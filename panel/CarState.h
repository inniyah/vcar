#ifndef CARSTATE_H_
#define CARSTATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <map>
#include <list>

#include "tinythread.h"
#include "intercom.h"

class CanMsgParser;

struct AnalogValue {
	uint64_t     RawValue;
	uint64_t     StartRawValue;
	double       Scale;
	double       Offset;
	bool         Signedness;
	const char * Units;
};

typedef std::map<std::string, std::map<std::string, AnalogValue > >             AnalogMap;
typedef std::map<std::string, AnalogValue >::iterator                           AnalogMapVarIterator;
typedef std::map<std::string, std::map<std::string, AnalogValue > >::iterator   AnalogMapGroupIterator;

typedef std::map<std::string, intercom::DataMessage::PwmMsg::Signal >           PwmMap;
typedef std::map<std::string, intercom::DataMessage::PwmMsg::Signal >::iterator PwmMapIterator;

struct ICarStateListener {
	virtual void eventCarStateChanged(void) = 0;
};

class CarState {
	friend class CanMsgParser;

public:
	CarState();
	virtual ~CarState();

	void printAnalogData();

	std::list<ICarStateListener*> listeners;
	AnalogMap analog_data;
	PwmMap    pwm_data;

private:
	bool stop;

	void receiveLoop();
	void sendLoop();

	intercom::Receiver receiver;

	static void receiveThreadFunc(void * arg);
	static void sendThreadFunc(void * arg);

	tthread::thread rcv_thread;
	tthread::thread snd_thread;
	CanMsgParser * car_msg_parser;
};

#endif // CARSTATE_H_
