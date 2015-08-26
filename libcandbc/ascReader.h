#ifndef INCLUDE_ASCREAD_H_8AE69578_4C11_11E5_95EC_10FEED04CD1C
#define INCLUDE_ASCREAD_H_8AE69578_4C11_11E5_95EC_10FEED04CD1C

/*  ascReader.h --  declarations for ascReader
	Copyright (C) 2007-2011 Andreas Heitmann

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include "model/dbcTypes.h"
#include <time.h>
#include "messageDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* ascRead function */
	void ascReader_processFile(FILE *fp, msgRxCb_t msgRxCb, void *cbData);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ASCREAD_H_8AE69578_4C11_11E5_95EC_10FEED04CD1C */
