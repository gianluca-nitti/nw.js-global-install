#include <iup.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "download.h"
#include "extractArchive.h"
#include "indexJsonFile.h"
#include "jsonFile.h"
#include "packageJsonFile.h"
#include "paths.h"
#include "strUtil.h"
#include "update.h"
#include "utils.h"
#ifdef _WIN32
	#include "win-only/IsWow64.h"
#else
	#include <fcntl.h>
#endif
#include "globals.h"
#include "downloaderGui.h"

static Ihandle *pb, *status;
int stop = 0;
int globalResult = DOWNLOADERGUI_ERROR;

int progressCb(long total, long now, double kBps){
	double progress = (double)now/(double)total;
	IupSetDouble(pb, "VALUE", progress);
	IupSetStrf(status, "TITLE", "Total file size: %dKb, downloaded: %dKb\n(progress: %d%%, average speed: %dkBps)", total/1000, now/1000, (int)(progress*100), (int)kBps);
	IupLoopStep();
	return stop;
}

int genericCb(long total, long now, double kBps){
	IupLoopStep();
	return 0;
}

int downloadCb(){
	IupSetFunction("IDLE_ACTION", NULL);
	IupLoopStep();
	char *cachePath = path_get_nwjs_cache();
	char *indexJsonPath = string_concat(2, cachePath, "../index.json");
	recursiveMkdir(cachePath, 0755); //Create directories if they don't already exist
	indexJsonFile_t versionIndex = {};
	if(download(INDEXJSON_URL, indexJsonPath, genericCb) != DOWNLOAD_SUCCESS){
		printf("[nwjsmanager][DEBUG] Failed to download versio index at URL '%s'. Cached version will be used if available.\n", INDEXJSON_URL);
	}
	indexJson_file_parse(indexJsonPath, &versionIndex);
	if(versionIndex.nwjsVersionCount != 0){
		if(update_required(&versionIndex)){
			Ihandle *info = IupGetHandle("info");
			IupSetStrf(info, "TITLE", "Downloading nwjsmanager update (you are running v%s, but v%d.%d.%d is available).", NWJSMANAGER_VERSION, versionIndex.nwjsmanagerLatestVersion.major, versionIndex.nwjsmanagerLatestVersion.minor, versionIndex.nwjsmanagerLatestVersion.patch);
			if(!update_install(&versionIndex, progressCb, _argc, _argv)){
				IupMessage(app->name, "Failed to install update!");
				return IUP_CLOSE;
			}
		}
		nwjsVersion_t *launchVersion = NULL;
		for(int i = 0; i < versionIndex.nwjsVersionCount; i++)
			if(packageJson_file_is_nw_version_OK(app, versionIndex.nwjsVersions[i].version) && (!launchVersion || semver_gt(versionIndex.nwjsVersions[i].version, launchVersion->version)))
				launchVersion = &versionIndex.nwjsVersions[i];
		if(!launchVersion){
			IupMessage(app->name, "Error: no nw.js version compatible with the application was found. Please try to reinstall the application, or contact the developer if the problem persists.");
		}else{
			printf("[nwjsmanager][DEBUG] Downloading nw.js %d.%d.%d\n", launchVersion->version.major, launchVersion->version.minor, launchVersion->version.patch);
			char *versionName = malloc(255); //Using 255 as max length (see https://github.com/mojombo/semver/blob/master/semver.md#does-semver-have-a-size-limit-on-the-version-string)
			sprintf(versionName, "v%d.%d.%d", launchVersion->version.major, launchVersion->version.minor, launchVersion->version.patch);
			nwjsDownload_t *downloads = &launchVersion->defaultDownloads;
			char *url;
			#ifdef _WIN32
				//Actually on Windows only a 32-bit binary is distributed since it will work out-of-the box thanks to Wow64. The IsWow64 function (see win-only/IsWow64.c) is used to detect at runtime if we are on a 64-bit system and properly select the nw.js architecture.
				if(IsWow64())
					url = downloads->win64;
				else
					url = downloads->win32;
			#else
				//On linux, architecture is detected at compile time because two different binaries are distributed (i386 and x86_64)
				#ifdef __x86_64__
					url = downloads->linux64;
				#else
					url = downloads->linux32;
				#endif
			#endif
			char *file = string_concat(2, cachePath, "download");
			int result = download(url, file, progressCb);
			if(result == DOWNLOAD_SUCCESS){
				result = extractArchive(file, cachePath);
				remove(file);
				char *oldName = strrchr(url, '/') + sizeof(char);
				char *oldNameEnd = strstr(oldName, ".zip");
				if(!oldNameEnd)
					oldNameEnd = strstr(oldName, ".tar.gz");
				*oldNameEnd = '\0';
				char *oldPath = string_concat(2, cachePath, oldName);
				*oldNameEnd = '.';
				char *newPath = string_concat(2, cachePath, versionName);
				rename(oldPath, newPath);
				free(oldPath);
				#ifndef _WIN32
					recursiveChmod(newPath, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);
				#endif
				free(newPath);
				globalResult = DOWNLOADERGUI_SUCCESS;
			}
			free(file);
			free(versionName);
		}
	}else
		IupMessage(app->name, "Error: this application requires the nw.js runtime, but a suitable version of the runtime files isn't installed. An attempt was done to download it, but it wasn't possible to retrive the version index.\nPlease check your Internet connection (this is necessary only for the first run of the application). Also, be sure the current user is allowed to write to the nw.js binary cache directory.");
	indexJson_file_free(&versionIndex);
	free(indexJsonPath);
	free(cachePath);
	return IUP_CLOSE;
}

void cancelCb(){
	if(IupAlarm("nwjsmanager", "If you stop the download of the nw.js runtime files, you won't be able to run the application.\nAre you sure?", "Yes", "No", NULL) == 1)
		stop = 1;
}



//Downloads und installs the latest supported nw.js version for the specified application.
int downloaderGui_download(){
	Ihandle *downloaderGui = IupGetHandle("downloaderDlg");
	pb = IupGetHandle("pb");
	status = IupGetHandle("status");
	IupSetCallback(IupGetHandle("cancel"), "ACTION", (Icallback)cancelCb);
	IupSetFunction("IDLE_ACTION", downloadCb);
	IupPopup(downloaderGui, IUP_CENTERPARENT, IUP_CENTERPARENT);
	return globalResult;
}
