#ifndef CARSTATE_H_
#define CARSTATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tinythread.h"

class CanMsgParser;

class CarState  {
public:
	CarState();
	virtual ~CarState();

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
