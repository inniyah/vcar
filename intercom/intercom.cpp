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
	switch (m_Message.Type) {
		case MsgText:
			if (NULL != m_Message.Data.Text.Message) {
				delete[] m_Message.Data.Text.Message;
			}
			break;
		default:
			break;
	}
	m_Message.Type = MsgEmpty;
}

void DataMessage::createTextMsg(const char * message, unsigned int length) {
	destroyMsg();
	if (NULL != message && 0 != length) {
		m_Message.Data.Text.Message = new char[length];
		memcpy(m_Message.Data.Text.Message, message, length);
		m_Message.Data.Text.Length = length;
	} else {
		m_Message.Data.Text.Message = NULL;
		m_Message.Data.Text.Length = 0;
	}
	m_Message.Type = MsgText;
}

void DataMessage::createCanMsg(const uint8_t payload[8]) {
	destroyMsg();
	memcpy(m_Message.Data.Can.Payload, payload, sizeof(CanMsg::Payload));
	m_Message.Type = MsgCan;
}


Sender::Sender() : m_isActive(false) {
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
}

Receiver::Receiver() : m_isActive(false) {
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
}


bool Sender::send(const DataMessage & msg_to_send) {
	if (!m_isActive) {
		return false;
	}

	const char * message="Hello, World!";

	/* now just sendto() our destination! */
	if (sendto(fd, message, strlen(message) + 1, 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("sendto");
		return false;
	}
	return true;
}

bool Receiver::receive(DataMessage & msg_rcv) {
	if (!m_isActive) {
		return false;
	}

	socklen_t addrlen;
	char msgbuf[INTERCOM_MAXMSGSIZE];
	addrlen = sizeof(addr);
	if ((nbytes = recvfrom(fd, msgbuf, INTERCOM_MAXMSGSIZE, 0, (struct sockaddr *) &addr, &addrlen)) < 0) {
		perror("recvfrom");
		return false;
	}
	fprintf(stderr, "\"%s\" (%lu)\n", msgbuf, (unsigned long)nbytes);
	return true;
}

}; // namespace Intercom
