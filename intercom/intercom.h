#ifndef INTERCOM_H_
#define INTERCOM_H_

#define INTERCOM_PORT         12345
#define INTERCOM_GROUP        "225.0.0.37"
#define INTERCOM_MAXMSGSIZE   256

#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace intercom {

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

	typedef struct MsgHeaderStruct {
		MsgType Type;
	} __attribute__((packed)) MsgHeader;

	typedef struct TextMsgStruct {
		uint16_t Length;
		uint8_t  Message[INTERCOM_MAXMSGSIZE - sizeof(MsgHeader)];
	} __attribute__((packed)) TextMsg;

	typedef struct CanMsgStruct {
		uint8_t Payload[8];
	} __attribute__((packed)) CanMsg;

	typedef struct FullMsgStruct {
		MsgHeader Header;
		union {
			TextMsg Text;
			CanMsg  Can;
		} __attribute__((packed)) Data;
	} __attribute__((packed)) FullMsg;

	void createTextMsg(const char * message, unsigned int length = 0);
	void createCanMsg(const uint8_t payload[8]);

	void fprint(FILE *stream) const;

protected:
	void destroyMsg(void);

private:
	FullMsg m_Message;
};
#pragma pack()

class Sender {
public:
	Sender();
	~Sender();

	bool send(const DataMessage & msg_to_send);

private:
	bool m_isActive;
	struct sockaddr_in addr;
	int fd;
};

class Receiver {
public:
	Receiver();
	~Receiver();

	bool receive(DataMessage & msg_rcv);

private:
	bool m_isActive;
	struct sockaddr_in addr;
	int fd;
	ssize_t nbytes;
	struct ip_mreq mreq;
};

}; // namespace intercom

#endif // INTERCOM_H_

