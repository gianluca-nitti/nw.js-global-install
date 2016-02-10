#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#define NWJSMANAGERPATH "/opt/nwjsmanager/nwjsmanager" //TODO: replace with a good location for nwjsmanager and define different paths for Windows and Linux

//Shows a message. Actually prints to stdout, but will be replaced with a platform-specific (GTK or WinAPI) message dialog.
void showMsg(char *msg){
	printf("%s\n", msg);
}

//Concatenates two strings.
char *concat(char *str1, char *str2){
	char *result = malloc((strlen(str1) + strlen(str2))*sizeof(char));
	strcpy(result, str1);
	return strcat(result, str2);
}

//Writes to buf #buflen bytes of the executable path
size_t getBinaryPath(char *buf, int bufLen){
	#ifdef _WIN32
		return GetModuleFileName(NULL, buf, bufLen);
	#else
		return readlink("/proc/self/exe", buf, bufLen);
	#endif
}

//Returns an absolute path to the directory in which the applauncher's executable is stored.
char *getBinaryDir(){
	int len = 1024*sizeof(char);
	char *binPath = NULL;
	do{ //the loop is to ensure that the string buffer we're allocating (binPath) is large enough to contain all the path (see http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html for information about pathname maximum size).
		len += 1024*sizeof(char);
		free(binPath);
		binPath = malloc(len*sizeof(char));
	}while(getBinaryPath(binPath, len) >= len);
	char *dirPath = dirname(binPath);
	return dirPath;
}

int main(int argc, char **argv){
	char *binDir = getBinaryDir();
	printf("[DEBUG] Application directory: %s\n", binDir);
	free(binDir);
	if(access("package.json", F_OK) != 0){ //TODO: path to package.json should be relative to the launcher's path and not to the CWD ad it's now.
		showMsg("Couldn't find package.json in (appname)'s directory. Please try reinstalling the application.");
		return 1;
	}
	char *cmd = concat(NWJSMANAGERPATH, " --version");
	if(system(cmd) != 0){
		showMsg("Couldn't find nwjsmanager, the tool required to launch (appname). Please try reinstalling the application.");
		free(cmd);
		return 1;
	}
	free(cmd);
	return 0;
}
