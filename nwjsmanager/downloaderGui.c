#include <iup.h>
#include <stdio.h>
#include "jsonFile.h"
#include "indexJsonFile.h"
#include "packageJsonFile.h"
#include "download.h"
#include "downloaderGui.h"

/*static int downloaderGui_close(Ihandle *btn){
	IupHide(IupGetDialog(btn));
	return IUP_DEFAULT;
}*/

static Ihandle *pb, *status;
static char* _url;
static char *_file;
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
	download(_url, _file, progressCb);
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
	Ihandle* downloaderGui = IupGetHandle("downloaderDlg");
	pb = IupGetHandle("pb");
	status = IupGetHandle("status");
	IupSetCallback(IupGetHandle("cancel"), "ACTION", (Icallback)cancelCb);
	IupSetFunction("IDLE_ACTION", downloadCb);
	indexJsonFile_t versionIndex = {};
	if(indexJson_file_parse("index.json", &versionIndex) != JSON_SUCCESS) //TODO:fix path
		getIndexJson(&versionIndex);
	//TODO: check for date and update from repository if it's obsolete.
	if(versionIndex.nwjsVersionCount != 0){
		semver_t latestNwVersion = *indexJson_file_get_latest_nwjs_version(&versionIndex);
		semver_t* launchVersion = NULL;
		for(int i = 0; i < versionIndex.nwjsVersionCount; i++)
			if(packageJson_file_is_nw_version_OK(app, versionIndex.nwjsVersions[i].version, latestNwVersion) && (!launchVersion || semver_gt(versionIndex.nwjsVersions[i].version, *launchVersion)))
				launchVersion = &versionIndex.nwjsVersions[i].version;
			//else
			//	printf("[nwjsmanager][DEBUG] Version %d.%d.%d is not compatible.\n", installedVersions.items[i].major, installedVersions.items[i].minor, installedVersions.items[i].patch);
		if(!launchVersion){
			printf("ERROR: no compatible nw.js version found!\n"); //TODO
		}else{
			printf("[nwjsmanager][DEBUG] Downloading nw.js %d.%d.%d\n", launchVersion->major, launchVersion->minor, launchVersion->patch);
			//_url = url;
			//_file = file;
			IupPopup(downloaderGui, IUP_CENTERPARENT, IUP_CENTERPARENT);
		}
	}else
		printf("ERROR: failed to load the nw.js version index!\n"); //TODO: replace with a dialog
	indexJson_file_free(&versionIndex);
	return 0; //TODO
}
