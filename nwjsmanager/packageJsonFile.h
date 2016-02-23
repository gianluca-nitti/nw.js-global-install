#ifndef _PACKAGEJSONFILE_H
#define _PACKAGEJSONFILE_H

#include <stdbool.h>
#define PACKAGEJSON_PARSE_ERROR -2

typedef struct{
	char *name;
	char *versionFilter;
	bool forceLatest;
}packageJsonFile_t;

int packageJson_file_parse(char *f, packageJsonFile_t *out);
void packageJson_file_free(packageJsonFile_t *f);

#endif
