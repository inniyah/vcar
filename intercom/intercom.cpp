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
	memcpy(m_Message.Data.Can.Payload, payload, dlc);
	m_Message.Header.Type = MsgCan;
}

void DataMessage::fprint(FILE *stream) const {
	switch(m_Message.Header.Type) {
		case MsgText:
			fprintf(stream, "[Text: \"%s\" (%lu) Src=%8lX]",
				m_Message.Data.Text.Message,
				(unsigned long)ntohs(m_Message.Data.Text.Length),
				(unsigned long)ntohl(m_Message.Header.SourceSys)
			);
			break;
		case MsgCan:
			fprintf(stream, "[CAN: 0x%lX: {", static_cast<unsigned long>(ntohl(m_Message.Data.Can.Id)));
			for (int i = 0; i < m_Message.Data.Can.Dlc; ++i) {
				fprintf(stream, " 0x%02X", m_Message.Data.Can.Payload[i]);
			}
			fprintf(stream, " } (%u) Src=%8lX]",
				m_Message.Data.Can.Dlc,
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
			msg_len = sizeof(DataMessage::MsgHeader) + sizeof(DataMessage::TextMsgStruct::Length) + ntohs(msg_info.Data.Text.Length);
			break;
		case DataMessage::MsgCan:
			msg_len = sizeof(DataMessage::MsgHeader) + 8;
			break;
		default:
			return true;
	}

	if (msg_len && msg_buffer) {
		msg_info.Header.SourceSys = htonl(m_SysId);

		//fputs("Snd Msg: ", stderr);
		//msg_to_send.fprint(stderr);
		//fputs("\n", stderr);

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

	return true;
}

}; // namespace Intercom
