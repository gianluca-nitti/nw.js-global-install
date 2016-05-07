#include <stdlib.h>
#include <string.h>
//#include <semver.h> //in the stable version of semver.c, semver.h doesn't have a proper include guard, so it's included only once inside packageJsonFile.h
#include "jsonFile.h"
#include "packageJsonFile.h"

static int is_semver_operator(char c){
	return c == '=' || c == '<' || c == '>' || c == '^' || c == '~';
}

//Cast the application's metadata from a package.json file to a packageJsonFile_t struct.
int packageJson_file_parse(char *f, packageJsonFile_t *out){
	jsonFile_t file = {};
	int result = json_file_parse(f, &file);
	if(result != JSON_SUCCESS){
		json_file_free(&file);
		return result;
	}
	out->name = json_file_get_value_from_key(&file, "name", 0);
	if(!out->name){
		json_file_free(&file);
		return PACKAGEJSON_PARSE_ERROR_NAME;
	}
	int nwjsmanager_root_token = json_file_get_token_index(&file, "nwjsmanager", 0);
	if(nwjsmanager_root_token == JSON_ERROR || nwjsmanager_root_token == file.tokenCount)
		out->versionFilter = NULL;
	else{
		char *versionFilter = json_file_get_value_from_key(&file, "nwjs-version-filter", nwjsmanager_root_token + 1);
		if(versionFilter){
			if(is_semver_operator(versionFilter[0])){
				int operatorLen = 1;
				out->versionFilterOperator[0] = versionFilter[0];
				if(is_semver_operator(versionFilter[1])){
					out->versionFilterOperator[1] = versionFilter[1];
					operatorLen++;
				}else
					out->versionFilterOperator[1] = '\0';
				out->versionFilter = malloc(sizeof(semver_t));
				if(semver_parse(versionFilter + operatorLen * sizeof(char), out->versionFilter) != 0){
					free(out->versionFilter);
					result = PACKAGEJSON_PARSE_ERROR_SEMVER;
				}
			}else
				result = PACKAGEJSON_PARSE_ERROR_SEMVER;
			free(versionFilter);
		}else
			out->versionFilter = NULL;
	}
	json_file_free(&file);
	return result;
}

//Free the memory associated with a packageJsonFile_t pointer.
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

//Check if the application associated with the provided package.json supports the provided NW.JS version.
int packageJson_file_is_nw_version_OK(packageJsonFile_t *f, semver_t nwVersion){
	return !f->versionFilter || semver_satisfies(nwVersion, *f->versionFilter, f->versionFilterOperator);
}
