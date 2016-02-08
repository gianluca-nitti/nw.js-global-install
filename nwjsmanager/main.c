#include <stdlib.h>
#include <iup.h>

Ihandle *mainDlg;

void led_load();

int main(int argc, char **argv){
	IupOpen(&argc, &argv);
	led_load(); //Loads the GUI compiled from LED files.
	mainDlg = IupGetHandle("mainDlg"); //Get the window named "mainDlg"
	IupShowXY(mainDlg, IUP_CENTER, IUP_CENTER); //Shows the main window
 	IupMainLoop();
 	IupClose();
	return EXIT_SUCCESS;
}
