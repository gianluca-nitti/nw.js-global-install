#ifndef _PACKAGEJSONFILE_H
#define _PACKAGEJSONFILE_H

#include <stdbool.h>
#include <semver.h>
#define PACKAGEJSON_PARSE_ERROR_SEMVER -2

typedef struct{
	char *name;
	semver_t *versionFilter;
	char versionFilterOperator[2];
	bool forceLatest;
}packageJsonFile_t;

int packageJson_file_parse(char *f, packageJsonFile_t *out);
void packageJson_file_free(packageJsonFile_t *f);
int packageJson_file_is_nw_version_OK(packageJsonFile_t *f, semver_t nwVersion, semver_t latestNwVersion);

#endif
