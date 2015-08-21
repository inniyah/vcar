#ifndef INTERCOM_H_
#define INTERCOM_H_

#define INTERCOM_PORT         12345
#define INTERCOM_GROUP        "225.0.0.37"
#define INTERCOM_MAXMSGSIZE   256
#define INTERCOM_MAXCANDLEN   8

#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace intercom {

enum {
	Sys_Unknown = 0,
	Sys_Panel   = 1,
	Sys_Ecu     = 2,
};

typedef uint32_t SysId; /* system identifier, to know what is coming from who */

#pragma pack(1)
class DataMessage {
	friend class Sender;
	friend class Receiver;

public:
	typedef enum MsgTypeEnum {
		MsgEmpty,
		MsgText,
		MsgCan
	} MsgType;

	typedef struct MsgHeaderStruct { // All fields in network endianness - use htonl or htons
		MsgType Type;
		SysId   SourceSys; // to be used by the sender function
	} __attribute__((packed)) MsgHeader;

	typedef struct TextMsgStruct { // All fields in network endianness - use htonl or htons
		uint16_t Length;
		uint8_t  Message[INTERCOM_MAXMSGSIZE - sizeof(MsgHeader)];
	} __attribute__((packed)) TextMsg;

	typedef uint32_t CanId;

	static const CanId CAN_EFF_FLAG = 0x80000000U; /* EFF/SFF is set in the MSB */
	static const CanId CAN_RTR_FLAG = 0x40000000U; /* remote transmission request */
	static const CanId CAN_ERR_FLAG = 0x20000000U; /* error message frame */

	typedef struct CanMsgStruct { // All fields in network endianness - use htonl or htons
		CanId   Id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
		uint8_t Bus;
		uint8_t Dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
		uint8_t Payload[INTERCOM_MAXCANDLEN];
	} __attribute__((packed)) CanMsg;

	typedef struct FullMsgStruct {
		MsgHeader Header;
		union {
			TextMsg Text;
			CanMsg  Can;
		} __attribute__((packed)) Data;
	} __attribute__((packed)) FullMsg;

	void createTextMsg(const char * message, unsigned int length = 0);
	void createCanMsg(CanId id, uint8_t dlc = 0, const uint8_t * payload = NULL, uint8_t bus = 0);

	MsgType getMsgType() {
		return m_Message.Header.Type;
	}

	SysId getSysId() {
		return ntohl(m_Message.Header.SourceSys);
	}

	CanMsg * getCanInfo() {
		if (MsgCan == m_Message.Header.Type) {
			return &m_Message.Data.Can;
		} else {
			return NULL;
		}
	}

	void fprint(FILE *stream) const;

protected:
	void destroyMsg(void);

private:
	FullMsg m_Message;
};
#pragma pack()

class Sender {
public:
	Sender(SysId sys_id = 0);
	~Sender();

	bool send(DataMessage & msg_to_send);

	SysId getSysId() {
		return m_SysId;
	}

private:
	SysId m_SysId;
	bool m_isActive;
	struct sockaddr_in addr;
	int fd;
};

class Receiver {
public:
	Receiver(SysId sys_id = 0);
	~Receiver();

	bool receive(DataMessage & msg_rcv);
	void shutdown();

	SysId getSysId() {
		return m_SysId;
	}

private:
	SysId m_SysId;
	bool m_isActive;
	struct sockaddr_in addr;
	int fd;
	ssize_t nbytes;
	struct ip_mreq mreq;
};

}; // namespace intercom

#endif // INTERCOM_H_

