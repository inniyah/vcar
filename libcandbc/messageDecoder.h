#ifndef INCLUDE_MESSAGEDECODER_H_8AE69550_4C11_11E5_95EB_10FEED04CD1C
#define INCLUDE_MESSAGEDECODER_H_8AE69550_4C11_11E5_95EB_10FEED04CD1C

#include "model/dbcModel.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* CAN message type */
	typedef struct {
		struct {
			time_t tv_sec;
			uint32 tv_nsec;
		} t;         /* time stamp */
		uint8   bus; /* can bus */
		uint32  id;  /* numeric CAN-ID */
		uint8   dlc;
		uint8   byte_arr[8];
	} canMessage_t;

	/* message received callback function */
	typedef void (* msgRxCb_t)(canMessage_t *message, void *cbData);

	/* signal procesing callback function */
	typedef void (* signalProcCb_t)(
		message_t * dbc_msg,
		canMessage_t * can_msg,
		const signal_t * s,
		uint32 rawValue,
		double physicalValue,
		void * cbData
	);

	void canMessage_decode(message_t * dbcMessage, canMessage_t * canMessage, signalProcCb_t signalProcCb, void * cbData);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_MESSAGEDECODER_H_8AE69550_4C11_11E5_95EB_10FEED04CD1C */
