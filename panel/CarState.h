#ifndef CARSTATE_H_
#define CARSTATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <map>

#include "tinythread.h"

class CanMsgParser;

struct AnalogValue {
	uint64_t RawValue;
	double Scale;
	double Offset;
	bool Unsigned;
};

typedef std::map<std::string, std::map<std::string, AnalogValue > > AnalogMap;
typedef std::map<std::string, AnalogValue >::iterator AnalogMapVarIterator;
typedef std::map<std::string, std::map<std::string, AnalogValue > >::iterator AnalogMapGroupIterator;

class CarState {
	friend class CanMsgParser;

public:
	CarState();
	virtual ~CarState();

	void printAnalogData();

	AnalogMap analog_data;

private:
	bool stop;

	void receiveLoop();
	void sendLoop();

	static void receiveThreadFunc(void * arg);
	static void sendThreadFunc(void * arg);

	tthread::thread rcv_thread;
	tthread::thread snd_thread;
	CanMsgParser * car_msg_parser;
};

#endif // CARSTATE_H_
