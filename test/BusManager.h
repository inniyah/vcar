#ifndef BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C
#define BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C

#include "Pdu.h"

#include "common/Singleton.h"
#include "common/CircularBuffer.h"
#include "common/LinkedList.h"

class BusManager : public common::Singleton<BusManager> {
public:
	BusManager();
	~BusManager();
	void init();
	void shutdown();

private:
	static BusManager sys; // Make sure that the object instance is created

	typedef Pdu<uint32_t, 8> Pdu8;
	common::CircularBuffer<Pdu8, 64> m_buffer_pdu8;
};

#endif // BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C
