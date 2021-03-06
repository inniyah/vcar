#ifndef INCLUDE_BUSASSIGNMENT_H_8AE695D2_4C11_11E5_95EE_10FEED04CD1C
#define INCLUDE_BUSASSIGNMENT_H_8AE695D2_4C11_11E5_95EE_10FEED04CD1C

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "messageHash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int bus;
	char * filename;
	messageHash_t * messageHash;
} busAssignmentEntry_t;

typedef struct {
	int n;
	busAssignmentEntry_t * list; /* array of n busAssigmentEntry_t's */
} busAssignment_t;

busAssignment_t * busAssignment_create(void);
void busAssignment_associate(busAssignment_t * busAssigment, int bus, const char * filename);
void busAssignment_free(busAssignment_t * busAssigment);
void busAssignment_print(busAssignment_t * busAssignment);

int busAssignment_parseDBC(busAssignment_t * busAssignment);

typedef void(*busAssignmentMessageCallback)(int can_bus, message_t * can_msg, void * arg);
typedef void(*busAssignmentSignalCallback)(int can_bus, message_t * can_msg, signal_t * can_sgn, void * arg);
typedef void(*busAssignmentAttributeCallback)(int can_bus, message_t * can_msg, signal_t * can_sgn, attribute_t * sgn_att, void * arg);

void busAssignment_iterate(
	busAssignment_t * busAssignment,
	busAssignmentMessageCallback msg_cb,
	busAssignmentSignalCallback sgn_cb,
	busAssignmentAttributeCallback att_cb,
	void * arg
);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BUSASSIGNMENT_H_8AE695D2_4C11_11E5_95EE_10FEED04CD1C */
