#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <gtk/gtk.h>
#endif
#define NWJSMANAGERPATH "/opt/nwjsmanager/nwjsmanager " //TODO: replace with a good location for nwjsmanager and define different paths for Windows and Linux

//Shows a message. Requires command line arguments because they are required by gtk_init.
void showMsg(int argc, char **argv, char *msg, char *appName){
	char *message = malloc((strlen(msg) - 1 + strlen(appName))*sizeof(char));
	sprintf(message, msg, appName); //TODO: remove .exe on windows
	#ifdef _WIN32
		MessageBox(NULL, message, appName, MB_ICONERROR);
	#else
		gtk_init(&argc, &argv);
		GtkWidget *msgBox = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Error while launching application");
		gtk_window_set_title(GTK_WINDOW(msgBox), appName);
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msgBox), "%s", message);
        	gtk_dialog_run(GTK_DIALOG(msgBox));
	#endif
	free(message);
}

//Concatenates two strings.
char *concat(char *str1, char *str2){
	char *result = malloc((strlen(str1) + strlen(str2) + 1)*sizeof(char));
	strcpy(result, str1);
	return strcat(result, str2);
}

//Writes to buf #buflen bytes of the executable path
size_t getBinaryPathBuffer(char *buf, int bufLen){
	#ifdef _WIN32
		return GetModuleFileName(NULL, buf, bufLen);
	#else
		return readlink("/proc/self/exe", buf, bufLen);
	#endif
}

//Returns an absolute path to the directory in which the applauncher's executable is stored.
char *getBinaryPath(){
	int len = 0;
	char *binPath = NULL;
	do{ //The loop is to ensure that the string buffer we're allocating (binPath) is large enough to contain all the path (see http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html for information about pathname maximum size).
		len += 1024;
		if(binPath)
			free(binPath);
		binPath = malloc(len*sizeof(char));
	}while(getBinaryPathBuffer(binPath, len) >= len);
	return binPath;
}

int main(int argc, char **argv){
	char *binPath = getBinaryPath();
	char *binPath_copy = strcpy(malloc((strlen(binPath) + 1)*sizeof(char)), binPath);
	char *binDir = dirname(binPath);
	char *binName = basename(binPath_copy);
	printf("[DEBUG] Application directory: %s\n[DEBUG] Application name: %s\n", binDir, binName);
	char *packageJsonPath = concat(binDir, "/package.json");
	printf("[DEBUG] Application's package.json path: %s\n", packageJsonPath);
	if(access(packageJsonPath, F_OK) != 0){
		showMsg(argc, argv, "Couldn't find package.json in %s's directory. Please try reinstalling the application.", binName);
		free(packageJsonPath);
		free(binPath);
		free(binPath_copy);
		return 1;
	}
	free(packageJsonPath);
	char *cmd = concat(NWJSMANAGERPATH, "--version");
	if(system(cmd) != 0){
		showMsg(argc, argv, "Couldn't find nwjsmanager, the tool required to launch %s. Please try reinstalling the application.", binName);
		free(cmd);
		free(binPath);
		free(binPath_copy);
		return 1;
	}
	free(cmd);
	cmd = concat(NWJSMANAGERPATH, binDir);
	free(binPath);
	free(binPath_copy);
	//Command line arguments are passed to nwjsmanager.
	int argIndex;
	for(argIndex = 1; argIndex < argc; argIndex++){
		char *arg = concat(" ", argv[argIndex]);
		char *oldCmd = cmd;
		cmd = concat(oldCmd, arg);
		free(oldCmd);
		free(arg);
	}
	printf("[DEBUG] Launch command: %s\n", cmd);
	int result = system(cmd);
	free(cmd);
	return result;
}
