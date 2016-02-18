#ifndef BUS_MANAGER_H_
#define BUS_MANAGER_H_

#include "common/Singleton.h"
#include "common/LinkedList.h"

class BusManager : public common::Singleton<BusManager> {
public:
	BusManager();
	~BusManager();
	void init();
	void shutdown();

private:
	static BusManager sys; // Make sure that the object instance is created
};

#endif // BUS_MANAGER_H_
