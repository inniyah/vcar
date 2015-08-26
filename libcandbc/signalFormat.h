#ifndef INCLUDE_SIGNALFORMAT_H_8AE69500_4C11_11E5_95E9_10FEED04CD1C
#define INCLUDE_SIGNALFORMAT_H_8AE69500_4C11_11E5_95E9_10FEED04CD1C

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	signalFormat_Name     = 1<<1,
	signalFormat_Message  = 1<<2,
	signalFormat_Database = 1<<3,
} signalFormat_t;

char *signalFormat_stringAppend(const char * in, const char * app);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SIGNALFORMAT_H_8AE69500_4C11_11E5_95E9_10FEED04CD1C */
