#ifndef RTE_H_
#define RTE_H_

#include <stdint.h>

namespace CanBus_can01 {
	struct TxMsgs;
	struct RxMsgs;
}

struct Rte {
	struct InStruct {
		uint8_t    Gear;
		uint8_t    Acceleration;
		uint8_t    Brake;
		int16_t    SteeringWheel;
	} In;
	struct OutStruct {
		uint8_t    InteriorLights;
		uint8_t    BrakeLights;
		uint8_t    BackwardsLights;
		uint8_t    LeftHazardLights;
		uint8_t    RightHazardLights;
	} Out;
	void init();
	void readInputs(CanBus_can01::RxMsgs & can01_rx);
	void writeOutputs(CanBus_can01::TxMsgs & can01_tx);
};

#endif
