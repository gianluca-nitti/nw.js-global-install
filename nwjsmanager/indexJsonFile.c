#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>
//#include <semver.h> //in the stable version of semver.c, semver.h doesn't have a proper include guard, so it's included only once inside indexJsonFile.h
#include "jsonFile.h"
#include "indexJsonFile.h"

static int indexJson_file_parse_downloads(jsonFile_t *file, int versionTokenIndex, char *type, nwjsDownload_t *out){
	int downloads_root_token = json_file_get_subtoken_abs_index(file, json_file_get_token_index(file, type, versionTokenIndex), 0);
	if(downloads_root_token == JSON_ERROR){
		out->linux32 = out->linux64 = out->win32 = out->win64 = NULL;
		return JSON_ERROR;
	}
	out->linux32 = json_file_get_value_from_key(file, "linux32", downloads_root_token);
	out->linux64 = json_file_get_value_from_key(file, "linux64", downloads_root_token);
	out->win32 = json_file_get_value_from_key(file, "win32", downloads_root_token);
	out->win64 = json_file_get_value_from_key(file, "win64", downloads_root_token);
	if(out->linux32 == NULL || out->linux64 == NULL || out->win32 == NULL || out->win64 == NULL)
		return JSON_ERROR;
	return JSON_SUCCESS;
}

//Cast the nwjs version information from the index.json file to a indexJsonFile_t struct.
int indexJson_file_parse(char *f, indexJsonFile_t *out){
	jsonFile_t file = {};
	int result = json_file_parse(f, &file);
	if(result != JSON_SUCCESS){
		json_file_free(&file);
		return result;
	}
	//Parse nwjsmanager update information.
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
	free(nwjsmanagerLatestVersion);
	int urgentDownloadToken = json_file_get_subtoken_abs_index(&file, json_file_get_token_index(&file, "nwjsmanager-update-urgent", 0), 0);
	out->nwjsmanagerUrgentUpdate = urgentDownloadToken != JSON_ERROR && json_file_get_token_value_boolean(&file, urgentDownloadToken);
	//Parse nw.js version list.
	int nwjs_versions_root_token = json_file_get_subtoken_abs_index(&file, json_file_get_token_index(&file, "nwjs-version-index", 0), 0);
	if(nwjs_versions_root_token == JSON_ERROR){
		json_file_free(&file);
		return INDEXJSON_ERROR_NWJS_INDEX;
	}
	int *nwjs_versions = json_file_get_subtokens_indices(&file, nwjs_versions_root_token);
	if(!nwjs_versions){
		json_file_free(&file);
		return INDEXJSON_ERROR_NWJS_INDEX;
	}
	out->nwjsVersionCount = file.tokens[nwjs_versions_root_token].size;
	out->nwjsVersions = malloc(out->nwjsVersionCount * sizeof(nwjsVersion_t));
	for(int i = 0; i < out->nwjsVersionCount; i++){
		char *v = json_file_get_value_from_key(&file, "version", nwjs_versions[i]);
		if(semver_parse(v, &out->nwjsVersions[i].version) == 0){
			if(indexJson_file_parse_downloads(&file, nwjs_versions[i], "default", &out->nwjsVersions[i].defaultDownloads) != JSON_SUCCESS)
				result = INDEXJSON_ERROR_NWJS_INDEX; //if there are no default downloads, then fail.
			//Other downloads are optional because not all nw.js versions are available in multiple build flavors.
			indexJson_file_parse_downloads(&file, nwjs_versions[i], "nacl", &out->nwjsVersions[i].naclDownloads);
			indexJson_file_parse_downloads(&file, nwjs_versions[i], "sdk", &out->nwjsVersions[i].sdkDownloads);
		}else
			result = INDEXJSON_ERROR_NWJS_INDEX;
		free(v);
		if(result != JSON_SUCCESS) break;
	}
	free(nwjs_versions);
	json_file_free(&file);
	return result;
}

static void indexJson_file_free_downloads(nwjsDownload_t *d){
	if(d->linux32) free(d->linux32);
	if(d->linux64) free(d->linux64);
	if(d->win32) free(d->win32);
	if(d->win64) free(d->win64);
}

//Free the memory associated with a indexJsonFile_t pointer.
void indexJson_file_free(indexJsonFile_t *f){
	if(f){
		semver_free(&f->nwjsmanagerLatestVersion);
		if(f->nwjsVersions){
			for(int i = 0; i < f->nwjsVersionCount; i++){
				semver_free(&f->nwjsVersions[i].version);
				indexJson_file_free_downloads(&f->nwjsVersions[i].defaultDownloads);
				indexJson_file_free_downloads(&f->nwjsVersions[i].naclDownloads);
				indexJson_file_free_downloads(&f->nwjsVersions[i].sdkDownloads);
			}
			free(f->nwjsVersions);
		}
		f->nwjsVersionCount = 0;
	}
}
