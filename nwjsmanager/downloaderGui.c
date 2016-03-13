#include <iup.h>
#include <stdio.h>
#include "download.h"
#include "downloaderGui.h"

/*static int downloaderGui_close(Ihandle *btn){
	IupHide(IupGetDialog(btn));
	return IUP_DEFAULT;
}*/

static Ihandle* pb;
static char* _url;
static char *_file;

int progressCb(long total, long now, double kBps){
	double progress = (double)now/(double)total;
	IupSetDouble(pb, "VALUE", progress);
	IupLoopStep();
	return 0;
}

int downloadCb(){
	IupSetFunction("IDLE_ACTION", NULL);
	download(_url, _file, progressCb);
	return IUP_CLOSE;
}

int downloaderGui_download(char *url, char *file){
	Ihandle* downloaderGui = IupGetHandle("downloaderDlg");
	pb = IupGetHandle("pb");
	_url = url;
	_file = file;
	IupSetFunction("IDLE_ACTION", downloadCb);
	IupPopup(downloaderGui, IUP_CENTERPARENT, IUP_CENTERPARENT);
	return 0; //TODO
}
