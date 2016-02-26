#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>
//#include <semver.h> //in the stable version of semver.c, semver.h doesn't have a proper include guard, so it's included only once inside indexJsonFile.h
#include "jsonFile.h"
#include "indexJsonFile.h"

//Cast the nwjs version information from the index.json file to a indexJsonFile_t struct.
int indexJson_file_parse(char *f, indexJsonFile_t *out){
	jsonFile_t file;
	int result = json_file_parse(f, &file);
	if(result != JSON_SUCCESS){
		json_file_free(&file);
		return result;
	}
	char *nwjsmanagerLatestVersion = json_file_get_value_from_key(&file, "nwjsmanager-latest-version", 0);
	if(!nwjsmanagerLatestVersion){
		json_file_free(&file);
		return INDEXJSON_ERROR_NWJSMANAGER_VERSION;
	}
	if(semver_parse(nwjsmanagerLatestVersion, &out->nwjsmanagerLatestVersion) != 0){
		free(nwjsmanagerLatestVersion);
		json_file_free(&file);
		return INDEXJSON_ERROR_NWJSMANAGER_VERSION;
	}
	json_file_free(&file);
	return result;
}

//Free the memory associated with a indexJsonFile_t pointer.
void indexJson_file_free(indexJsonFile_t *f){
	if(f){
		semver_free(&f->nwjsmanagerLatestVersion);
	}
}
