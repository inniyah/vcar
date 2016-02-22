#ifndef BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C
#define BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C

#include "Pdu.h"

#include "common/Singleton.h"
#include "common/SafeCircularBuffer.h"
#include "common/LinkedList.h"

class BusManager : public common::Singleton<BusManager> {
public:
	BusManager();
	~BusManager();
	void init();
	void shutdown();

	typedef uint32_t BusId;
	typedef uint32_t PduId;

private:
	static BusManager sys; // Make sure that the object instance is created

	typedef Pdu<PduId, 8> Pdu8;
	common::SafeCircularBuffer<Pdu8, 64> m_buffer_pdu8;
};

#endif // BUS_MANAGER_H_7D9F1978_D666_11E5_86AB_10FEED04CD1C
