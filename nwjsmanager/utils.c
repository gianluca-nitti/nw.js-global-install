#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "strUtil.h"
#include "utils.h"

//Ignore mode to mkdir on windows
#ifdef _WIN32
	#define mkdir(X, Y) mkdir(X)
#endif

int recursiveMkdir(char *pathname, int mode){
	char *p;
	int r;
	//if (pathname[strlen(pathname) - 1] == '/')
	//	pathname[strlen(pathname) - 1] = '\0';
	r = mkdir(pathname, mode);
	if (r != 0) {
		/* On failure, try creating parent directory. */
		p = strrchr(pathname, '/');
		if (p != NULL) {
			*p = '\0';
			recursiveMkdir(pathname, 0755);
			*p = '/';
			r = mkdir(pathname, mode);
		}
	}
	return r;
}

#ifndef _WIN32
void recursiveChmod(char *path, mode_t mode){
	if(chmod(path, mode))
		return; //exit if failed to chmod the root
	DIR *d = opendir(path);
	if(!d)
		return;
	struct dirent *ent;
	while((ent = readdir(d)) != NULL){
		if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
			char *entPath = string_concat(3, path, "/", ent->d_name);
			recursiveChmod(entPath, mode);
			free(entPath);
		}
	}
	closedir(d);
}
#endif
