#ifndef INTERCOM_H_
#define INTERCOM_H_

#define INTERCOM_PORT        12345
#define INTERCOM_GROUP       "225.0.0.37"
#define INTERCOM_MAXMSGSIZE  256

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace intercom {

class DataMessage {
public:

	typedef enum MsgTypeEnum {
		MsgEmpty,
		MsgText,
		MsgCan
	} MsgType;

	typedef struct TextMsgStruct {
		char * Message;
		unsigned int Length;
	} TextMsg;

	typedef struct CanMsgStruct {
		uint8_t Payload[8];
	} CanMsg;

	typedef struct FullMsgStruct {
		MsgType Type;
		union {
			TextMsg Text;
			CanMsg  Can;
		} Data;
	} FullMsg;

	void createTextMsg(const char * message, unsigned int length);
	void createCanMsg(const uint8_t payload[8]);

protected:
	void destroyMsg(void);

private:
	FullMsg m_Message;
};

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

