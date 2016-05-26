#include <semver.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "download.h"
#include "strUtil.h"
#include "indexJsonFile.h"
#include "paths.h"
#include "update.h"
#ifndef _WIN32
	#include <sys/stat.h>
#endif

bool update_required(indexJsonFile_t *indexJson){
	semver_t currentVersion = {};
	semver_parse(NWJSMANAGER_VERSION, &currentVersion);
	return semver_compare(indexJson->nwjsmanagerLatestVersion, currentVersion) > 0 && indexJson->nwjsmanagerUrgentUpdate;
}

bool update_install(indexJsonFile_t *indexJson, int (*downloadCallback)(long total, long now, double kBps), int argc, char **argv){
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
	char *binPath = getBinaryPath();
	printf("[nwjsmanager][DEBUG] Update download URL: %s\n[nwjsmanager][DEBUG] Current binary path: %s\n", url, binPath);
	#ifndef _WIN32
		struct stat st;
		stat(binPath, &st);
	#endif
	char *renamedBinPath = string_concat(2, binPath, ".old");
	rename(binPath, renamedBinPath);
	bool result = download(url, binPath, downloadCallback) == DOWNLOAD_SUCCESS;
	if(!result)
		rename(renamedBinPath, binPath);
	free(renamedBinPath);
	free(url);
	#ifndef _WIN32
		chmod(binPath, st.st_mode); //make the new version executable by copying the permissions from the previous version
	#endif
	if(result){
		//Duplicate array and add null pointer to it's end (as required by execv)
		char **args = calloc(argc + 1, sizeof(char*));
		for(int i = 0; i < argc; i++)
			args[i] = argv[i];
		args[argc] = NULL;
		execv(binPath, argv);
	}
	free(binPath);
	return result;
}
