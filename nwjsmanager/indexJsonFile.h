#ifndef _INDEXJSONFILE_H
#define _INDEXJSONFILE_H

#include <stdbool.h>
#ifndef __SEMVER_H
#include <semver.h>
#endif

#define INDEXJSON_ERROR_NWJSMANAGER_VERSION -2
#define INDEXJSON_ERROR_NWJS_INDEX -3

typedef struct{
	char *linux32;
	char *linux64;
	char *win32;
	char *win64;
}nwjsDownload_t;

typedef struct{
	semver_t version;
	nwjsDownload_t defaultDownloads;
	nwjsDownload_t naclDownloads;
	nwjsDownload_t sdkDownloads;
}nwjsVersion_t;

typedef struct{
	semver_t nwjsmanagerLatestVersion;
	int nwjsVersionCount;
	nwjsVersion_t *nwjsVersions;
}indexJsonFile_t;

int indexJson_file_parse(char *f, indexJsonFile_t *out);
semver_t* indexJson_file_get_latest_nwjs_version(indexJsonFile_t *f);
void indexJson_file_free(indexJsonFile_t *f);

#endif
