#include <stdlib.h>
#include <iup.h>
#include <stdio.h>

Ihandle *mainDlg;

void led_load();

int main(int argc, char **argv){
	for(int i = 0; i < argc; i++)
		printf("[nwjsmanager][DEBUG] argv[%d]: %s\n", i, argv[i]);
	//TODO: parse package.json and serch for a compatible locally installed nw.js version.
	IupOpen(&argc, &argv);
	led_load(); //Loads the GUI compiled from LED files.
	mainDlg = IupGetHandle("mainDlg"); //Get the window named "mainDlg"
	IupShowXY(mainDlg, IUP_CENTER, IUP_CENTER); //Shows the main window
 	IupMainLoop();
 	IupClose();
	return EXIT_SUCCESS;
}
