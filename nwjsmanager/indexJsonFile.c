#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>
//#include <semver.h> //in the stable version of semver.c, semver.h doesn't have a proper include guard, so it's included only once inside indexJsonFile.h
#include "strUtil.h"
#include "jsonFile.h"
#include "indexJsonFile.h"

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
		char *v = json_file_get_token_value(&file, nwjs_versions[i]);
		if(semver_parse(v, &out->nwjsVersions[i].version) == 0){
			char *baseUrl = string_concat(4, "http://dl.nwjs.io/v", v, "/nwjs-v", v, "");
			out->nwjsVersions[i].defaultDownloads.linux32 = string_concat(2, baseUrl, "-linux-ia32.tar.gz");
			out->nwjsVersions[i].defaultDownloads.linux64 = string_concat(2, baseUrl, "-linux-x64.tar.gz");
			out->nwjsVersions[i].defaultDownloads.win32 = string_concat(2, baseUrl, "-win-ia32.zip");
			out->nwjsVersions[i].defaultDownloads.win64 = string_concat(2, baseUrl, "-win-x64.zip");
			free(baseUrl);
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
			}
			free(f->nwjsVersions);
		}
		f->nwjsVersionCount = 0;
	}
}
