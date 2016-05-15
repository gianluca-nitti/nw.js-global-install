#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <shlobj.h>
#else
	#include <unistd.h>
#endif
#include "paths.h"

char* path_get_nwjs_cache(){
	#ifdef _WIN32
		char *path = malloc(MAX_PATH * sizeof(char));
		SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, path);
		path = strcat(path, "\\nwjs\\bin\\");
		return path;
	#else
		return strdup("/var/cache/nwjs/bin/");
	#endif
}

static size_t getBinaryPathBuffer(char *buf, int bufLen){
	#ifdef _WIN32
		return GetModuleFileName(NULL, buf, bufLen);
	#else
		return readlink("/proc/self/exe", buf, bufLen);
	#endif
}

//Returns an absolute path to the directory in which the nwjsmanager's executable is stored.
char *getBinaryPath(){
	int len = 0;
	int finalLen = 0;
	char *binPath = NULL;
	do{ //The loop is to ensure that the string buffer we're allocating (binPath) is large enough to contain all the path (see http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html for information about pathname maximum size).
		len += 2;
		if(binPath)
			free(binPath);
		binPath = malloc(len*sizeof(char));
	}while((finalLen = getBinaryPathBuffer(binPath, len)) >= len);
	binPath[finalLen] = '\0';
	return binPath;
}

/*char *getParentDirectory(char *filePath){
	for(int i = strlen(filePath) - 1; i >= 0; i--)
		if(filePath[i] == '/' || filePath[i] == '\\'){
			char *result = malloc((i + 1) * sizeof(char));
			strncpy(result, filePath, i);
			result[i] = '\0';
			return result;
		}
	return NULL;
}*/
