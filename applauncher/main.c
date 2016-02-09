#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

int main(int argc, char **argv){
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
