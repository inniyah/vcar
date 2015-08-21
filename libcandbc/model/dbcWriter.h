#ifndef INCLUDE_DBCWRITER_H
#define INCLUDE_DBCWRITER_H

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

#endif
