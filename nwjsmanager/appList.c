#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strUtil.h"
#include "paths.h"
#include "textFile.h"

static char* getAppListPath(){
	char* dirPath = path_get_nwjs_cache();
	char* listPath = string_concat(2, dirPath, "../appList");
	free(dirPath);
	return listPath;
}

static char** readAppList(){
	char* appListPath = getAppListPath();
	char* appListStr = readTextFile(appListPath);
	free(appListPath);
	if(!appListStr)
		appListStr = strdup(""); //If the file doesn't exists, the list is initialized empty.
	int len = strlen(appListStr);
	int lines = 0;
	for(int i = 0; i < len; i++)
		if(appListStr[i] == '\n')
			lines++;
	char** list = malloc((lines + 2) * sizeof(char*));
	list[lines] = NULL;
	list[lines + 1] = NULL; //Two null bytes are added at the end of the list. One marks the end, and the first one can be replaced by a pointer to the string to add.
	int lineIndex = 0;
	int lineLen = 0;
	int lineStart = 0;
	for(int i = 0; i < len; i++)
		if(appListStr[i] == '\n'){
			list[lineIndex] = malloc(lineLen + 1);
			strncpy(list[lineIndex], appListStr + lineStart, lineLen);
			list[lineIndex][lineLen] = '\0';
			lineIndex++;
			lineLen = 0;
			lineStart = i + 1;
		}else{
			lineLen++;
		}
	free(appListStr);
	return list;
}

static void freeAppList(char** list){
	int i = -1;
	while(list[++i])
		free(list[i]);
	free(list);
}

static int writeAppList(char** list){
	int i = -1;
	int fileSize = 1;
	while(list[++i])
		fileSize += strlen(list[i]) + 1;
	char* appListStr = malloc(fileSize);
	appListStr[0] = '\0';
	i = -1;
	while(list[++i])
		strcat(strcat(appListStr, list[i]), "\n");
	char* appListPath = getAppListPath();
	int result = writeTextFile(appListPath, appListStr);
	free(appListPath);
	free(appListStr);
	return result;
}

//Adds an application to the list of the installed applications.
void appList_add(char *name){
	char** appList = readAppList();
	int i = 0;
	while(appList[i])
		if(strcmp(appList[i++], name) == 0){
			freeAppList(appList);
			return;
		}
	appList[i] = strdup(name); //This replaces the first null byte; the second one becomes the marker of the end of the list.
	writeAppList(appList);
	freeAppList(appList);
}

//Removes an application from the list. Returns 1 if after the operation the list is empty, otherwise 0. This will be used to prompt the user if he/she wants to remove nwjsmanager and nw.js cached binaries when uninstalling an application (the prompt will be shown only if there aren't other nw.js applications).
int appList_remove(char *name){
	char** appList = readAppList();
	int i = 0;
	while(appList[i])
		if(strcmp(appList[i++], name) == 0){
			free(appList[--i]);
			while(appList[i++])
				appList[i - 1] = appList[i];
			break;
		}
	writeAppList(appList);
	freeAppList(appList);
	return 0;
}
