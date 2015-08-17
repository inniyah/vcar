#ifndef INCLUDE_MEASUREMENT_H
#define INCLUDE_MEASUREMENT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include "hashtable/hashtable.h"
#include "busAssignment.h"
#include "signalFormat.h"
#include "messageDecoder.h"

/* message received callback function */
typedef void (* msgRxCb_t)(canMessage_t *message, void *cbData);

typedef struct
{
	struct hashtable *timeSeriesHash;
} measurement_t;

typedef struct
{
	unsigned int n;
	double *time;
	double *value;
} timeSeries_t;

typedef void (* parserFunction_t)(FILE *fp, msgRxCb_t msgRxCb, void *cbData);

measurement_t *measurement_read(busAssignment_t *busAssignment,
const char *filename,
signalFormat_t signalFormat,
sint32 timeResolution,
parserFunction_t parserFunction);

void measurement_free(measurement_t *m);
#endif
