#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "test.h"
#include "../jsonFile.h"
#include "../packageJsonFile.h"
#include "../indexJsonFile.h"
#include "../nwjsBinaryCache.h"
#include "../download.h"
#include "../extractArchive.h"
#include "../update.h"

int parseSimpleJson(){
	jsonFile_t f;
	if(json_file_parse("test.json", &f) != JSON_SUCCESS)
		return 0;
	int i = json_file_get_token_index(&f, "key2", 0);
	int j = json_file_get_token_index(&f, "value2", 0); //Should return an error, because value2 is nested into key2
	if(i == JSON_ERROR)
		return 0;
	if(j != JSON_ERROR)
		return 0;
	char *val = json_file_get_token_value(&f, i);
	json_file_free(&f);
	if(!val)
		return 0;
	int result = strcmp(val, "key2") == 0;
	free(val);
	return result;
}

int parsePackageJson(){ //TODO: fix memory leaks (when returning failures, memory isn't always freed)
	jsonFile_t f;
	if(json_file_parse("package.json", &f) != JSON_SUCCESS)
		return 0;
	char *version = json_file_get_value_from_key(&f, "version", 0);
	if(!version)
		return 0;
	char *start = json_file_get_value_from_key(&f, "start", 0); //should be NULL because it's nested
	if(start){
		free(start);
		free(version);
		return 0;
	}
	int deps = json_file_get_token_index(&f, "dependencies", 0);
	if(deps == JSON_ERROR)
		return 0;
	int window = json_file_get_token_index(&f, "window", 0);
	if(window == JSON_ERROR)
		return 0;
	int toolbar = json_file_get_token_index(&f, "toolbar", window + 1);
	if(toolbar == JSON_ERROR || !json_file_token_is_boolean(&f, toolbar + 1) || json_file_get_token_value_boolean(&f, toolbar + 1))
		return 0;
	int nwjsmanager_root_token = json_file_get_token_index(&f, "nwjsmanager", 0);
	if(nwjsmanager_root_token == JSON_ERROR)
		return 0;
	nwjsmanager_root_token = json_file_get_subtoken_abs_index(&f, nwjsmanager_root_token, 0);
	if(nwjsmanager_root_token == JSON_ERROR)
		return 0;
	int *nwjsmanager_values = json_file_get_subtokens_indices(&f, nwjsmanager_root_token);
	if(!nwjsmanager_values)
		return 0;
	int nw_version_index = nwjsmanager_values[0];
	free(nwjsmanager_values);
	char *nw_ver = json_file_get_value_from_key(&f, "nw", deps + 1);
	char *nwjsmanager_version_filter = json_file_get_token_value(&f, nw_version_index);
	int result = strcmp(version, "1.0.0") == 0 && strcmp(nw_ver, "^0.12.2") == 0 && strcmp(nwjsmanager_version_filter, "nwjs-version-filter") == 0;
	json_file_free(&f);
	free(version);
	free(nw_ver);
	free(nwjsmanager_version_filter);
	return result;
}

int parseAppPackageJson(){
	packageJsonFile_t packageJson;
	int result = packageJson_file_parse("package.json", &packageJson);
	if(result != JSON_SUCCESS)
		return 0;
	if(!packageJson.versionFilter)
		return 0;
	semver_t unsupportedVersion, supportedVersion1, supportedVersion2;
	semver_parse("0.11.0", &unsupportedVersion);
	semver_parse("0.12.2", &supportedVersion1);
	semver_parse("0.13.1", &supportedVersion2);
	result = strcmp(packageJson.name, "nw-image-viewer") == 0
		&& packageJson.versionFilter->major == 0 && packageJson.versionFilter->minor == 12 && packageJson.versionFilter->patch == 2
		&& packageJson.versionFilterOperator[0] == '>' && packageJson.versionFilterOperator[1] == '='
		&& !packageJson_file_is_nw_version_OK(&packageJson, unsupportedVersion) && packageJson_file_is_nw_version_OK(&packageJson, supportedVersion1) && packageJson_file_is_nw_version_OK(&packageJson, supportedVersion2);
	packageJson_file_free(&packageJson);
	return result;
}

int parseIndexJson(){
	indexJsonFile_t indexJson;
	int result = indexJson_file_parse("../../index.json", &indexJson);
	if(result != JSON_SUCCESS){
		indexJson_file_free(&indexJson);
		return 0;
	}
	result = indexJson.nwjsmanagerLatestVersion.major == 1 && indexJson.nwjsmanagerLatestVersion.minor == 0 && indexJson.nwjsmanagerLatestVersion.patch == 0 && indexJson.nwjsmanagerUrgentUpdate;
	for(int i = 0; i < indexJson.nwjsVersionCount; i++){
		if(indexJson.nwjsVersions[i].version.major == 0 && indexJson.nwjsVersions[i].version.minor == 12)
			result &= indexJson.nwjsVersions[i].defaultDownloads.linux64 != NULL && indexJson.nwjsVersions[i].naclDownloads.win32 == NULL && indexJson.nwjsVersions[i].sdkDownloads.win32 == NULL;
		else if (indexJson.nwjsVersions[i].version.major == 0 && indexJson.nwjsVersions[i].version.minor == 13)
			result &= indexJson.nwjsVersions[i].defaultDownloads.linux32 != NULL && indexJson.nwjsVersions[i].naclDownloads.win64 != NULL && indexJson.nwjsVersions[i].sdkDownloads.win32 != NULL;
	}
	indexJson_file_free(&indexJson);
	return result;
}

static int versionEq(semver_t version, int major, int minor, int patch){
	return version.major == major && version.minor == minor && version.patch == patch;
}

int listInstalledNwjsVersions(){
	chdir("./chroot");
	if(chroot(".") != 0){
		puts("failed to enter chroot environment");
		return 0;
	}
	semverList_t versionList = nwjs_binary_cache_get_versions();
	int result = versionList.count == 2 && (versionEq(versionList.items[0], 0, 12, 1) || versionEq(versionList.items[0], 0, 13, 0))
		&& (versionEq(versionList.items[1], 0, 12, 1) || versionEq(versionList.items[1], 0, 13, 0));
	semverList_free(&versionList);
	return result;
}

int downloadCb(long total, long now, double kBps){
	printf("\rTotal bytes: %10lu, downloaded bytes: %10lu (%.2f%%), speed: %.2f kB/s   ", total, now, 100*(double)now/(double)total, kBps);
	return 0;
}

int downloadFile(){
	putchar('\n');
	return download("http://dl.nwjs.io/v0.12.3/chromedriver-nw-v0.12.3-linux-x64.tar.gz", "test-data/downloadedFile", downloadCb) == DOWNLOAD_SUCCESS;
}

int extract(){
	return extractArchive("test-data/downloadedFile", "test-data/extractDir") == ARCHIVE_SUCCESS;
}

int update(){
	indexJsonFile_t testIndexJson;
	int result = indexJson_file_parse("testIndex.json", &testIndexJson);
	if(result != JSON_SUCCESS){
		indexJson_file_free(&testIndexJson);
		return 0;
	}
	result = update_required(&testIndexJson);
	update_install(&testIndexJson, downloadCb);
	indexJson_file_free(&testIndexJson);
	return result;
}

int main(int argc, char **argv){
	test_init(8);
	test_add("Parse a simple JSON file", parseSimpleJson);
	test_add("Parse a package.json file", parsePackageJson);
	test_add("Cast an application's package.json file to a packageJsonFile_t struct", parseAppPackageJson);
	test_add("Parse the index.json file", parseIndexJson);
	test_add("Download a file (working Internet connection is required)", downloadFile);
	test_add("Extract an archive (.tar.gz on Linux, .zip on Windows) (requires the previous test was passed)", extract);
	test_add("Simulate an update", update);
	test_add("List locally installed nwjs versions in a simulated environment (note: sudo is required)", listInstalledNwjsVersions);
	return test_run();
}
