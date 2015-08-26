#ifndef INCLUDE_DBCWRITER_H_8AE69456_4C11_11E5_95E5_10FEED04CD1C
#define INCLUDE_DBCWRITER_H_8AE69456_4C11_11E5_95E5_10FEED04CD1C

#include "dbcModel.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void dbc_write(FILE *out, dbc_t *dbc);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DBCWRITER_H_8AE69456_4C11_11E5_95E5_10FEED04CD1C */
