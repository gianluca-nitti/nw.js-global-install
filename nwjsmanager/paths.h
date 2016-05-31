#ifndef _PATHS_H
#define _PATHS_H

#define INDEXJSON_URL "http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/index.json"

#ifdef _WIN32
	#define NWJS_BIN_NAME "nw.exe"
#else
	#define NWJS_BIN_NAME "nw"
#endif

char* path_get_nwjs_cache();
char *getBinaryPath();
char *getParentDirectory(char *filePath);

#endif
