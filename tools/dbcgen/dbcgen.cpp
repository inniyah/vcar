#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "model/dbcReader.h"
#include "model/dbcWriter.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	dbc_t *dbc;
	char * const inFilename = (argc>1 ? argv[1] : NULL);
	char * const outFilename = (argc>2 ? argv[2] : NULL);
	int ret = EXIT_SUCCESS;

	if (NULL != (dbc = dbc_read_file(inFilename))) {
		FILE * dbcout;

		if(outFilename != NULL)	{
			if(NULL == (dbcout = fopen(outFilename,"w"))) {
				fprintf(stderr, "can't open output file '%s'\n", outFilename);
				ret = EXIT_FAILURE;
			}
		} else {
			dbcout = stdout;
		}

		if(dbcout != NULL) {
			// TODO

			if(outFilename != NULL) {
				fclose(dbcout);
			}
		}

		dbc_free(dbc);
	} else {
		fprintf(stderr, "can't open input file '%s'\n", inFilename);
		ret = 1;
	}

	return ret;
}
