#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#define NWJSMANAGERPATH "/opt/nwjsmanager/nwjsmanager " //TODO: replace with a good location for nwjsmanager and define different paths for Windows and Linux

//Shows a message. Actually prints to stdout, but will be replaced with a platform-specific (GTK or WinAPI) message dialog.
void showMsg(char *msg){
	printf("%s\n", msg);
}

//Concatenates two strings.
char *concat(char *str1, char *str2){
	char *result = malloc((strlen(str1) + strlen(str2))*sizeof(char) + 1);
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
	int len = 0;
	char *binPath = NULL;
	do{ //The loop is to ensure that the string buffer we're allocating (binPath) is large enough to contain all the path (see http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html for information about pathname maximum size).
		len += 1024;
		if(binPath)
			free(binPath);
		binPath = malloc(len*sizeof(char));
	}while(getBinaryPath(binPath, len) >= len);
	char *dirPath = dirname(binPath);
	return dirPath;
}

int main(int argc, char **argv){
	char *binDir = getBinaryDir();
	printf("[DEBUG] Application directory: %s\n", binDir);
	char *packageJsonPath = concat(binDir, "/package.json");
	printf("[DEBUG] Application's package.json path: %s\n", packageJsonPath);
	if(access(packageJsonPath, F_OK) != 0){
		showMsg("Couldn't find package.json in (appname)'s directory. Please try reinstalling the application.");
		free(packageJsonPath);
		return 1;
	}
	free(packageJsonPath);
	char *cmd = concat(NWJSMANAGERPATH, "--version");
	if(system(cmd) != 0){
		showMsg("Couldn't find nwjsmanager, the tool required to launch (appname). Please try reinstalling the application.");
		free(cmd);
		return 1;
	}
	free(cmd);
	cmd = concat(NWJSMANAGERPATH, binDir);
	free(binDir);
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
