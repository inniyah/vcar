#ifndef INTERCOM_H_8AE6979E_4C11_11E5_95F9_10FEED04CD1C
#define INTERCOM_H_8AE6979E_4C11_11E5_95F9_10FEED04CD1C

#define INTERCOM_PORT         12345
#define INTERCOM_GROUP        "225.0.0.37"
#define INTERCOM_MAXMSGSIZE   256

/*
 * Typically, on little-endian architectures, the rightmost character of a multi-char will
 * become the least significant byte of the resulting integer. On big-endian architectures,
 * it is the other way around.
 */

#if (defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)) || defined(__BIG_ENDIAN__) || \
        (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__)) || \
        defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
        defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
    // It's a big-endian architecture
    #define FIX_MULTICHAR_STR4(C) ( C )
#elif (defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || defined(__LITTLE_ENDIAN__) || \
        (defined(__BYTE_ORDER__) && (__BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__)) || \
        defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
        defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
    // It's a little-endian architecture
    #define FIX_MULTICHAR_STR4(C) ( (((C) & 0x000000FFu) << 24) | (((C) & 0x0000FF00u) <<  8) | (((C) & 0x00FF0000u) >> 8) | (((C) & 0xFF000000u) >> 24) )
#else
    #error "I don't know what architecture this is!"
#endif

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
	Sys_Spy     = 3,
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
		MsgCan,
		MsgPwm,
		MsgAdc,
	} MsgType;

	typedef struct MsgHeaderStruct { // All fields in network endianness - use htonl or htons
		MsgType Type;
		SysId   SourceSys; // to be used by the sender function
	} __attribute__((packed)) MsgHeader;

	typedef struct TextMsgStruct { // All fields in network endianness - use htonl or htons
		static const uint16_t MAX_LENGTH = INTERCOM_MAXMSGSIZE - sizeof(MsgHeader);
		uint16_t Length;
		uint8_t  Message[MAX_LENGTH];
	} __attribute__((packed)) TextMsg;

	typedef uint32_t CanId;

	static const CanId CAN_EFF_FLAG = 0x80000000U; /* EFF/SFF is set in the MSB */
	static const CanId CAN_RTR_FLAG = 0x40000000U; /* remote transmission request */
	static const CanId CAN_ERR_FLAG = 0x20000000U; /* error message frame */

	typedef struct CanMsgStruct { // All fields in network endianness - use htonl or htons
		static const uint8_t MAX_DLC = 8;
		CanId   Id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
		uint8_t Bus;
		uint8_t Dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
		uint8_t Payload[MAX_DLC];
	} __attribute__((packed)) CanMsg;

	typedef struct PwmMsgStruct { // All fields in network endianness - use htonl or htons
		uint16_t Count;
		typedef struct SignalStruct { // All fields in network endianness - use htonl or htons
			uint32_t Id;
			uint16_t Period;
			uint16_t PulseWidth;
			inline double getIntensity() {
				return static_cast<double>(PulseWidth) / static_cast<double>(Period);
			}
		} __attribute__((packed)) Signal;
		static const int MAX_COUNT = (INTERCOM_MAXMSGSIZE - sizeof(MsgHeader) - sizeof(Count)) / sizeof(Signal);
		Signal Signals[MAX_COUNT];
	} __attribute__((packed)) PwmMsg;

	class PwmSignal : public PwmMsg::Signal {
	public:
		PwmSignal(uint32_t id, uint16_t pulse_width, uint16_t period) {
			Id = htonl(id);
			PulseWidth = htons(pulse_width);
			Period = htons(period);
		}
	};

	typedef struct AdcMsgStruct { // All fields in network endianness - use htonl or htons
		uint16_t Count;
		typedef struct SignalStruct { // All fields in network endianness - use htonl or htons
			uint32_t Id;
			uint32_t Value;
		} __attribute__((packed)) Signal;
		static const int MAX_COUNT = (INTERCOM_MAXMSGSIZE - sizeof(MsgHeader) - sizeof(Count)) / sizeof(Signal);
		Signal Signals[MAX_COUNT];
	} __attribute__((packed)) AdcMsg;

	class AdcSignal : public AdcMsg::Signal {
	public:
		AdcSignal(uint32_t id, uint32_t value) {
			Id = htonl(id);
			Value = htonl(value);
		}
	};

	typedef struct FullMsgStruct {
		MsgHeader Header;
		union {
			TextMsg Text;
			CanMsg  Can;
			PwmMsg  Pwm;
			AdcMsg  Adc;
		} __attribute__((packed)) Data;
	} __attribute__((packed)) FullMsg;

	void createTextMsg(const char * message, unsigned int length = 0);
	void createCanMsg(CanId id, uint8_t dlc = 0, const uint8_t * payload = NULL, uint8_t bus = 0);
	void createPwmMsg(const PwmMsg::Signal & signal);
	void createPwmMsg(uint16_t count, const PwmMsg::Signal signals[]);
	void createAdcMsg(const AdcMsg::Signal & signal);
	void createAdcMsg(uint16_t count, const AdcMsg::Signal signals[]);

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

	PwmMsg * getPwmInfo() {
		if (MsgPwm == m_Message.Header.Type) {
			return &m_Message.Data.Pwm;
		} else {
			return NULL;
		}
	}

	AdcMsg * getAdcInfo() {
		if (MsgAdc == m_Message.Header.Type) {
			return &m_Message.Data.Adc;
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

#endif // INTERCOM_H_8AE6979E_4C11_11E5_95F9_10FEED04CD1C
