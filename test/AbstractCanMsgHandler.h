#ifndef ABSTRACT_CAN_MSG_HANDLER_H_8AE69726_4C11_11E5_95F6_10FEED04CD1C
#define ABSTRACT_CAN_MSG_HANDLER_H_8AE69726_4C11_11E5_95F6_10FEED04CD1C

#include <stdlib.h>
#include <stdint.h>

class AbstractCanMsgHandler {
public:
	AbstractCanMsgHandler() { }
	virtual ~AbstractCanMsgHandler() { }
	virtual uint8_t * getCanMessageBuffer(uint32_t id, uint8_t * dlt = NULL) = 0;

	inline uint8_t * getCanMessageBufferWithDlt(uint32_t id, uint8_t dlt) {
		uint8_t msg_dlt;
		uint8_t * msg_buf = getCanMessageBuffer(id, &msg_dlt);
		return ( dlt == msg_dlt ? msg_buf : NULL );
	}
};

typedef AbstractCanMsgHandler AbstractCanTxMsgHandler;
typedef AbstractCanMsgHandler AbstractCanRxMsgHandler;

#endif // ABSTRACT_CAN_MSG_HANDLER_H_8AE69726_4C11_11E5_95F6_10FEED04CD1C
