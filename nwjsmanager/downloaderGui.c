#include <iup.h>
#include <stdio.h>
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

int downloaderGui_download(char *url, char *file){
	Ihandle* downloaderGui = IupGetHandle("downloaderDlg");
	pb = IupGetHandle("pb");
	status = IupGetHandle("status");
	_url = url;
	_file = file;
	IupSetCallback(IupGetHandle("cancel"), "ACTION", (Icallback)cancelCb);
	IupSetFunction("IDLE_ACTION", downloadCb);
	IupPopup(downloaderGui, IUP_CENTERPARENT, IUP_CENTERPARENT);
	return 0; //TODO
}
