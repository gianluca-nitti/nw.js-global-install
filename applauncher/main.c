#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#ifdef _WIN32
	#include <windows.h>
	#include <shlobj.h>
#else
	#include <gtk/gtk.h>
#endif

//Shows a message. Requires command line arguments because they are required by gtk_init.
void showMsg(int argc, char **argv, char *msg, char *appName){
	char *message = malloc((strlen(msg) - 1 + strlen(appName))*sizeof(char));
	sprintf(message, msg, appName);
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
	char *binPath_copy = strdup(binPath);
	char *binDir = dirname(binPath);
	char *binName = basename(binPath_copy);
	char *dot = strrchr(binName, '.'); //To remove the trailing .exe on Windows
	if(dot && !strcmp(dot, ".exe"))
		binName[strlen(binName) - 4] = 0;
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
	char *nwjsmanagerpath = getenv("NWJSMANAGERPATH");
	if(!nwjsmanagerpath){
		#ifdef _WIN32
			nwjsmanagerpath = (char[MAX_PATH]){};
			SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, nwjsmanagerpath);
			nwjsmanagerpath = strcat(nwjsmanagerpath, "\\nwjs\\nwjsmanager.exe");
		#else
			nwjsmanagerpath = "/usr/local/bin/nwjsmanager";
		#endif
	}
	printf("[DEBUG] Path to nwjsmanager: %s\n", nwjsmanagerpath);
	if(access(nwjsmanagerpath, X_OK) != 0){ //TODO: ensure it's not a directory
		showMsg(argc, argv, "Couldn't find nwjsmanager, the tool required to launch %s.\nIf you have set the NWJSMANAGERPATH environment variable, it is set to a path that doesn't exists or isn't an executable file; unset it to search for nwjsmanager in the default path. If you haven't set the variable and you just want to run the application, please try reinstalling the application.", binName);
		free(binPath);
		free(binPath_copy);
		return 1;
	}
	char **args = calloc(argc + 2, sizeof(char*));
	args[0] = nwjsmanagerpath;
	#ifdef _WIN32
		args[1] = malloc(strlen(binDir) + 3);
		strcpy(args[1], "\""); //Put the path in quotes because if it contains spaces it will interpreted as multiple arguments on Windows
		strcat(args[1], binDir);
		strcat(args[1], "\"");
	#else
		args[1] = strdup(binDir); //argv will be passed to nwjsmanager; the application directory is passed as the first command line argument
	#endif
	printf("[DEBUG] Path to application's directory: %s\n", args[1]);
	for(int i = 1; i < argc; i++)
		args[i + 1] = argv[i];
	args[argc + 1] = NULL;
	free(binPath);
	free(binPath_copy);
	return execv(nwjsmanagerpath, args);
}
