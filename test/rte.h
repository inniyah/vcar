#ifndef RTE_H_8AE69820_4C11_11E5_95FC_10FEED04CD1C
#define RTE_H_8AE69820_4C11_11E5_95FC_10FEED04CD1C

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
};

#endif // RTE_H_8AE69820_4C11_11E5_95FC_10FEED04CD1C

