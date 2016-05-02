#include <string.h>
#ifdef _WIN32
	#include <shlobj.h>
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
