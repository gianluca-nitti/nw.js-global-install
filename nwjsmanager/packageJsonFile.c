#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <semver.h> //in the stable version of semver.c, semver.h doesn't have a proper include guard, so it's included only once inside packageJsonFile.h
#include "jsonFile.h"
#include "packageJsonFile.h"

//Cast the application's metadata from a package.json file to a packageJsonFile_t struct.
int packageJson_file_parse(char *f, packageJsonFile_t *out){
	jsonFile_t file;
	int result = json_file_parse(f, &file);
	if(result != JSON_SUCCESS){
		json_file_free(&file);
		return result;
	}
	out->name = json_file_get_value_from_key(&file, "name", 0);
	if(!out->name)
		out->name = strdup("(unknown name application)");
	int nwjsmanager_root_token = json_file_get_token_index(&file, "nwjsmanager", 0);
	if(nwjsmanager_root_token == JSON_ERROR || nwjsmanager_root_token == file.tokenCount){
		out->versionFilter = NULL;
		out->forceLatest = false;
	}else{
		char *versionFilter = json_file_get_value_from_key(&file, "nwjs-version-filter", nwjsmanager_root_token + 1);
		if(versionFilter){
			out->versionFilter = malloc(sizeof(semver_t));
			if(semver_parse(versionFilter, out->versionFilter) != 0){
				free(out->versionFilter);
				out->versionFilter = NULL;
			}
			free(versionFilter);
		}else
			out->versionFilter = NULL;
		int forceLatest = json_file_get_token_index(&file, "force-latest", nwjsmanager_root_token + 1);
		if(json_file_token_is_boolean(&file, forceLatest + 1))
			out->forceLatest = json_file_get_token_value_boolean(&file, forceLatest + 1);
		else
			out->forceLatest = false;
	}
	json_file_free(&file);
	return JSON_SUCCESS;
}

void packageJson_file_free(packageJsonFile_t *f){
	if(f){
		if(f->name)
			free(f->name);
		if(f->versionFilter){
			semver_free(f->versionFilter);
			free(f->versionFilter);
		}
	}
}
