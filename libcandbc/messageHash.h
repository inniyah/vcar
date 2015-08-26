#ifndef INCLUDE_MESSAGEHASH_H_8AE69528_4C11_11E5_95EA_10FEED04CD1C
#define INCLUDE_MESSAGEHASH_H_8AE69528_4C11_11E5_95EA_10FEED04CD1C

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include "model/dbcModel.h"
#include "hashtable/hashtable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32 messageHashKey_t;
typedef struct hashtable messageHash_t;

struct hashtable *messageHash_create(message_list_t * ml);
void messageHash_free(messageHash_t * const h);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_MESSAGEHASH_H_8AE69528_4C11_11E5_95EA_10FEED04CD1C */
