#ifndef SYS_COM_H_09103170_D5C4_11E5_A2AA_10FEED04CD1C
#define SYS_COM_H_09103170_D5C4_11E5_A2AA_10FEED04CD1C

#include "tinythread.h"
#include "intercom.h"

class SysComHandler {
public:
	SysComHandler();
	virtual ~SysComHandler();

private:
	bool stop;

	void receiveLoop();
	void sendLoop();

	intercom::Receiver receiver;

	static void receiveThreadFunc(void * arg);
	static void sendThreadFunc(void * arg);

	tthread::thread rcv_thread;
	tthread::thread snd_thread;
};

#endif // SYS_COM_H_09103170_D5C4_11E5_A2AA_10FEED04CD1C

