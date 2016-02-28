#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "paths.h"
#include "nwjsBinaryCache.h"

semverList_t nwjs_binary_cache_get_versions(){
	semverList_t result = {NULL, 0};
	char *binDir = path_get_nwjs_cache();
	//char *binDir = strdup("../../");
	DIR *d = opendir(binDir);
	if(!d){
		free(binDir);
		return result;
	}
	struct dirent *ent;
	int allocatedSize = 10;
	result.items = malloc(allocatedSize * sizeof(semver_t));
	while((ent = readdir(d)) != NULL){
		if(strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
			continue;
		struct stat st;
		char *filename = malloc((strlen(binDir) + strlen(ent->d_name) + 2) * sizeof(char)); //TODO: could be replace with an ad-hoc function, like the concat() used in applauncher
		strcpy(filename, binDir);
		strcat(filename, "/");
		strcat(filename, ent->d_name);
		if(stat(filename, &st) != 0){
			free(filename);
			continue;
		}
		free(filename); //TODO: memory leak to fix
		if(S_ISDIR(st.st_mode) && strlen(ent->d_name) > 1 && ent->d_name[0] == 'v' && semver_parse(ent->d_name + sizeof(char), &result.items[result.count]) == 0){
			result.count++;
			if(result.count == allocatedSize)
				result.items = realloc(result.items, (allocatedSize = result.count + 1) * sizeof(semver_t));
		}
	}
	closedir(d);
	free(binDir);
	return result;
}

void semverList_free(semverList_t *l){
	if(l){
		for(int i = 0; i < l->count; i++)
			semver_free(&l->items[i]);
		l->count = 0;
		if(l->items)
			free(l->items);
	}
}
