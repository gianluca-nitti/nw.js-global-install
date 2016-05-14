#include <semver.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "indexJsonFile.h"
#include "update.h"

bool update_required(indexJsonFile_t *indexJson){
	semver_t currentVersion = {};
	semver_parse(NWJSMANAGER_VERSION, &currentVersion);
	return semver_compare(indexJson->nwjsmanagerLatestVersion, currentVersion) > 0 && indexJson->nwjsmanagerUrgentUpdate;
}

bool update_install(indexJsonFile_t *indexJson, int (*downloadCallback)(long total, long now, double kBps)){
	char versionString[255];
	sprintf(versionString, "%d.%d.%d", indexJson->nwjsmanagerLatestVersion.major, indexJson->nwjsmanagerLatestVersion.minor, indexJson->nwjsmanagerLatestVersion.patch);
	#ifdef _WIN32
		char *suffix = ".exe";
	#else
		#ifdef __x86_64__
			char *suffix = "-linux64";
		#else
			char *suffix = "-linux32";
		#endif
	#endif
	char *url = malloc((strlen(UPDATE_URL) - 3 + strlen(versionString) + strlen(suffix))*sizeof(char)); //-3 because of subtracting placeholders (the two %s, 4 chars) and adding the null terminator.
	sprintf(url, UPDATE_URL, versionString, suffix);
	printf("[nwjsmanager][DEBUG] Update download URL: %s", url);
	free(url);
	return false; //TODO
}
