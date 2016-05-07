#ifndef _PACKAGEJSONFILE_H
#define _PACKAGEJSONFILE_H

#ifndef __SEMVER_H
#include <semver.h>
#endif
#define PACKAGEJSON_PARSE_ERROR_NAME -2
#define PACKAGEJSON_PARSE_ERROR_SEMVER -3

typedef struct{
	char *name;
	semver_t *versionFilter;
	char versionFilterOperator[2];
}packageJsonFile_t;

int packageJson_file_parse(char *f, packageJsonFile_t *out);
void packageJson_file_free(packageJsonFile_t *f);
int packageJson_file_is_nw_version_OK(packageJsonFile_t *f, semver_t nwVersion);

#endif
