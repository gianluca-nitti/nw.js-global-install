#include <iup.h>
#include <stdlib.h>
#include <stdio.h>
#include "download.h"
#include "indexJsonFile.h"
#include "jsonFile.h"
#include "packageJsonFile.h"
#include "paths.h"
#include "strUtil.h"
#ifdef _WIN32
	#include "win-only/IsWow64.h"
#endif
#include "downloaderGui.h"

/*static int downloaderGui_close(Ihandle *btn){
	IupHide(IupGetDialog(btn));
	return IUP_DEFAULT;
}*/

static Ihandle *pb, *status;
static char* url;
int stop = 0;

int progressCb(long total, long now, double kBps){
	double progress = (double)now/(double)total;
	IupSetDouble(pb, "VALUE", progress);
	IupSetStrf(status, "TITLE", "Total file size: %dKb, downloaded: %dKb\n(progress: %d%%, average speed: %dkBps)", total/1000, now/1000, (int)(progress*100), (int)kBps);
	IupLoopStep();
	return stop;
}

int downloadCb(){
	IupSetFunction("IDLE_ACTION", NULL);
	char *file = string_concat(2, path_get_nwjs_cache(), "download");
	download(url, file, progressCb);
	free(file);
	return IUP_CLOSE;
}

void cancelCb(){
	if(IupAlarm("nwjsmanager", "If you stop the download of the nw.js runtime files, you won't be able to run the application.\nAre you sure?", "Yes", "No", NULL) == 1)
		stop = 1;
}

static void getIndexJson(indexJsonFile_t *out){
	//TODO
}

//Downloads und installs the latest supported nw.js version for the specified application.
int downloaderGui_download(packageJsonFile_t *app){
	Ihandle *downloaderGui = IupGetHandle("downloaderDlg");
	pb = IupGetHandle("pb");
	status = IupGetHandle("status");
	IupSetCallback(IupGetHandle("cancel"), "ACTION", (Icallback)cancelCb);
	indexJsonFile_t versionIndex = {};
	if(indexJson_file_parse("index.json", &versionIndex) != JSON_SUCCESS) //TODO:fix path
		getIndexJson(&versionIndex);
	//TODO: check for date and update from repository if it's obsolete.
	if(versionIndex.nwjsVersionCount != 0){
		semver_t latestNwVersion = *indexJson_file_get_latest_nwjs_version(&versionIndex);
		nwjsVersion_t *launchVersion = NULL;
		for(int i = 0; i < versionIndex.nwjsVersionCount; i++)
			if(packageJson_file_is_nw_version_OK(app, versionIndex.nwjsVersions[i].version, latestNwVersion) && (!launchVersion || semver_gt(versionIndex.nwjsVersions[i].version, launchVersion->version)))
				launchVersion = &versionIndex.nwjsVersions[i];
			//else
			//	printf("[nwjsmanager][DEBUG] Version %d.%d.%d is not compatible.\n", installedVersions.items[i].major, installedVersions.items[i].minor, installedVersions.items[i].patch);
		if(!launchVersion){
			printf("ERROR: no compatible nw.js version found!\n"); //TODO
		}else{
			printf("[nwjsmanager][DEBUG] Downloading nw.js %d.%d.%d\n", launchVersion->version.major, launchVersion->version.minor, launchVersion->version.patch);
			nwjsDownload_t *downloads = &launchVersion->defaultDownloads;
			//TODO: use sdk or nacl build when required.
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
			IupSetFunction("IDLE_ACTION", downloadCb);
			IupPopup(downloaderGui, IUP_CENTERPARENT, IUP_CENTERPARENT);
		}
	}else
		printf("ERROR: failed to load the nw.js version index!\n"); //TODO: replace with a dialog
	indexJson_file_free(&versionIndex);
	return 0; //TODO
}
