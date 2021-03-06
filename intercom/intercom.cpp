#include "intercom.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

namespace intercom {

void DataMessage::destroyMsg(void) {
	m_Message.Header.Type = MsgEmpty;
}

void DataMessage::createTextMsg(const char * message, unsigned int length) {
	destroyMsg();
	if ((NULL != message) && (0 == length)) {
		length = strlen(message) + 1; // include the terminating null byte ('\0')
	}
	if (length > sizeof(m_Message.Data.Text.Message)) { // limit length
		length = sizeof(m_Message.Data.Text.Message);
	}
	if (NULL != message && 0 != length) {
		memcpy(m_Message.Data.Text.Message, message, length);
		m_Message.Data.Text.Length = htons(length);
	} else {
		m_Message.Data.Text.Message[0] = '\0';
		m_Message.Data.Text.Length = htons(0);
	}
	m_Message.Header.Type = MsgText;
}

void DataMessage::createCanMsg(CanId id, uint8_t dlc, const uint8_t * payload, uint8_t bus) {
	destroyMsg();
	m_Message.Data.Can.Id = htonl(id);
	m_Message.Data.Can.Dlc = dlc;
	m_Message.Data.Can.Bus = bus;
	if (NULL != payload) {
		memcpy(m_Message.Data.Can.Payload, payload, dlc);
	} else {
		memset(m_Message.Data.Can.Payload, 0, sizeof(m_Message.Data.Can.Payload));
	}
	m_Message.Header.Type = MsgCan;
}

void DataMessage::createPwmMsg(const PwmMsg::Signal & signal) {
	destroyMsg();
	m_Message.Data.Pwm.Count = htons(1);
	memset(m_Message.Data.Pwm.Signals, 0, sizeof(m_Message.Data.Pwm.Signals));
	m_Message.Data.Pwm.Signals[0] = signal;
	m_Message.Header.Type = MsgPwm;
}

void DataMessage::createPwmMsg(uint16_t count, const PwmMsg::Signal signals[]) {
	destroyMsg();
	if (count > 0 && count < PwmMsg::MAX_COUNT) {
		m_Message.Data.Pwm.Count = htons(count);
		memset(m_Message.Data.Pwm.Signals, 0, sizeof(m_Message.Data.Pwm.Signals));
		for (int i = 0; i < count; ++i) {
			m_Message.Data.Pwm.Signals[i] = signals[i];
		}
	} else {
		m_Message.Data.Pwm.Count = 0;
	}
	m_Message.Header.Type = MsgPwm;
}

void DataMessage::createAdcMsg(const AdcMsg::Signal & signal) {
	destroyMsg();
	m_Message.Data.Adc.Count = htons(1);
	memset(m_Message.Data.Adc.Signals, 0, sizeof(m_Message.Data.Adc.Signals));
	m_Message.Data.Adc.Signals[0] = signal;
	m_Message.Header.Type = MsgAdc;
}

void DataMessage::createAdcMsg(uint16_t count, const AdcMsg::Signal signals[]) {
	destroyMsg();
	if (count > 0 && count < AdcMsg::MAX_COUNT) {
		m_Message.Data.Adc.Count = htons(count);
		memset(m_Message.Data.Adc.Signals, 0, sizeof(m_Message.Data.Adc.Signals));
		for (int i = 0; i < count; ++i) {
			m_Message.Data.Adc.Signals[i] = signals[i];
		}
	} else {
		m_Message.Data.Adc.Count = 0;
	}
	m_Message.Header.Type = MsgAdc;
}

void DataMessage::fprint(FILE *stream) const {
	switch(m_Message.Header.Type) {
		case MsgText:
			fprintf(stream, "[Text: \"%s\" (%lu/%lu) Src=%8lX]",
				m_Message.Data.Text.Message,
				(unsigned long)ntohs(m_Message.Data.Text.Length),
				(unsigned long)m_Message.Data.Text.MAX_LENGTH,
				(unsigned long)ntohl(m_Message.Header.SourceSys)
			);
			break;
		case MsgCan:
			fprintf(stream, "[CAN: 0x%lX: {", static_cast<unsigned long>(ntohl(m_Message.Data.Can.Id)));
			for (int i = 0; i < m_Message.Data.Can.Dlc; ++i) {
				fprintf(stream, " 0x%02X", m_Message.Data.Can.Payload[i]);
			}
			fprintf(stream, " } (%u/%u) Bus=%d Src=%8lX]",
				m_Message.Data.Can.Dlc,
				m_Message.Data.Can.MAX_DLC,
				m_Message.Data.Can.Bus,
				(unsigned long)ntohl(m_Message.Header.SourceSys)
			);
			break;
		case MsgPwm:
			fprintf(stream, "[PWM: {");
			for (int i = 0; i < ntohs(m_Message.Data.Pwm.Count); ++i) {
				uint32_t id = FIX_MULTICHAR_STR4(ntohl(m_Message.Data.Pwm.Signals[i].Id));
				fprintf(stream, " %.*s=%u/%u",
					4, reinterpret_cast<const char *>(&id),
					ntohs(m_Message.Data.Pwm.Signals[i].PulseWidth),
					ntohs(m_Message.Data.Pwm.Signals[i].Period)
				);
			}
			fprintf(stream, " } (%u/%u) Src=%8lX]",
				ntohs(m_Message.Data.Pwm.Count),
				m_Message.Data.Pwm.MAX_COUNT,
				(unsigned long)ntohl(m_Message.Header.SourceSys)
			);
			break;
		case MsgAdc:
			fprintf(stream, "[ADC: {");
			for (int i = 0; i < ntohs(m_Message.Data.Adc.Count); ++i) {
				uint32_t id = FIX_MULTICHAR_STR4(ntohl(m_Message.Data.Adc.Signals[i].Id));
				fprintf(stream, " %.*s=%u",
					4, reinterpret_cast<const char *>(&id),
					ntohl(m_Message.Data.Adc.Signals[i].Value)
				);
			}
			fprintf(stream, " } (%u/%u) Src=%8lX]",
				ntohs(m_Message.Data.Adc.Count),
				m_Message.Data.Adc.MAX_COUNT,
				(unsigned long)ntohl(m_Message.Header.SourceSys)
			);
			break;
		default:
			break;
	}
}


Sender::Sender(SysId sys_id) :
		m_SysId( ((::getpid() & 0x000000FFlu) << 24) | (sys_id & 0x00FFFFFFlu)),
		m_isActive(false)
{
	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		return;
	}

/*
     TTL    Scope
    ----------------------------------------------------------------------
       0    Restricted to the same host. Won't be output by any interface.
       1    Restricted to the same subnet. Won't be forwarded by a router.
     <32    Restricted to the same site, organization or department.
     <64    Restricted to the same region.
    <128    Restricted to the same continent.
    <255    Unrestricted in scope. Global.
*/

	const int ttl = 1; /* max = 255 */

	if (setsockopt(fd,IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		perror("Setting TTL failed");
	}

	/* set up destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(INTERCOM_GROUP);
	addr.sin_port = htons(INTERCOM_PORT);

	m_isActive = true;
}

Sender::~Sender() {
	m_isActive = false;
}

Receiver::Receiver(SysId sys_id) :
		m_SysId( ((::getpid() & 0x000000FFlu) << 24) | (sys_id & 0x00FFFFFFlu)),
		m_isActive(false)
{
	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
		return;
	}

	/* allow multiple sockets to use the same PORT number */
	u_int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("Reusing ADDR failed");
		return;
	}

	/* set up destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
	addr.sin_port = htons(INTERCOM_PORT);

	/* bind to receive address */
	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
		perror("bind");
		return;
	}

	/* use setsockopt() to request that the kernel join a multicast group */
	mreq.imr_multiaddr.s_addr=inet_addr(INTERCOM_GROUP);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
		perror("setsockopt");
		return;
	}

	m_isActive = true;
}

Receiver::~Receiver() {
	m_isActive = false;
}


bool Sender::send(DataMessage & msg_to_send) {
	if (!m_isActive) {
		return false;
	}

	DataMessage::FullMsg & msg_info = msg_to_send.m_Message;
	const char * msg_buffer = reinterpret_cast<const char *>(&msg_info);
	size_t msg_len = 0;

	switch(msg_info.Header.Type) {
		case DataMessage::MsgText:
			msg_len =
				sizeof(DataMessage::MsgHeader)
				+ sizeof(DataMessage::TextMsgStruct::Length)
				+ ntohs(msg_info.Data.Text.Length);
			break;
		case DataMessage::MsgCan:
			msg_len =
				sizeof(DataMessage::MsgHeader)
				+ sizeof(DataMessage::CanMsgStruct)
				- sizeof(DataMessage::CanMsgStruct::Payload)
				+ msg_info.Data.Can.Dlc;
			break;
		case DataMessage::MsgPwm:
			msg_len =
				sizeof(DataMessage::MsgHeader)
				+ sizeof(DataMessage::PwmMsgStruct)
				- sizeof(DataMessage::PwmMsgStruct::Signal) * (
						msg_info.Data.Pwm.MAX_COUNT - ntohs(msg_info.Data.Pwm.Count)
					);
			break;
		case DataMessage::MsgAdc:
			msg_len =
				sizeof(DataMessage::MsgHeader)
				+ sizeof(DataMessage::AdcMsgStruct)
				- sizeof(DataMessage::AdcMsgStruct::Signal) * (
						msg_info.Data.Adc.MAX_COUNT - ntohs(msg_info.Data.Adc.Count)
					);
			break;
		default:
			return true;
	}

	if (msg_len && msg_buffer) {
		msg_info.Header.SourceSys = htonl(m_SysId);

		fputs("Snd Msg: ", stderr);
		msg_to_send.fprint(stderr);
		fprintf(stderr," (size=%lu)\n", (unsigned long)msg_len);

		/* now just sendto() our destination! */
		if (sendto(fd, msg_buffer, msg_len, 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("sendto");
			return false;
		}
		return true;
	} else {
		return false;
	}
}

bool Receiver::receive(DataMessage & msg_rcv) {
	if (!m_isActive) {
		return false;
	}

	DataMessage::FullMsg & msg_info = msg_rcv.m_Message;
	char * msg_buffer = reinterpret_cast<char *>(&msg_info);
	size_t msg_len = sizeof(DataMessage::FullMsg);

	socklen_t addrlen;
	addrlen = sizeof(addr);
	if ((nbytes = recvfrom(fd, msg_buffer, msg_len, 0, (struct sockaddr *) &addr, &addrlen)) < 0) {
		perror("recvfrom");
		return false;
	}
	//fprintf(stderr, "Bytes received:%ld (max expected: %lu)\n", (long int)nbytes, (long int)msg_len);

#if 0
	if (htonl(m_SysId) != msg_info.Header.SourceSys) {
		fputs("Msg Rcv: ", stderr);
		msg_rcv.fprint(stderr);
		fputs("\n", stderr);
	} else {
		fputs("Msg Ign: ", stderr);
		msg_rcv.fprint(stderr);
		fputs("\n", stderr);
		//msg_info.Header.Type = DataMessage::MsgEmpty;
	}
#endif
	return true;
}

void Receiver::shutdown() {
	::shutdown(fd, SHUT_RDWR);
}


}; // namespace Intercom
