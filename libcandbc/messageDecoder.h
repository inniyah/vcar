#ifndef INCLUDE_MESSAGEDECODER_H
#define INCLUDE_MESSAGEDECODER_H

#include "dbcModel.h"

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
		const signal_t * s,
		double dtime,
		uint32 rawValue,
		double physicalValue,
		void * cbData
	);

	void canMessage_decode(
		message_t      * dbcMessage,
		canMessage_t   * canMessage,
		sint32           timeResolution,
		signalProcCb_t   signalProcCb,
		void           * cbData
	);

#ifdef __cplusplus
}
#endif

#endif
