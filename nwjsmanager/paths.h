#ifndef _PATHS_H
#define _PATHS_H

#define INDEXJSON_URL "https://raw.githubusercontent.com/gianluca-nitti/nw.js-global-install/master/index.json"

#ifdef _WIN32
	#define NWJS_BIN_NAME "nw.exe"
#else
	#define NWJS_BIN_NAME "nw"
#endif

char* path_get_nwjs_cache();

#endif
