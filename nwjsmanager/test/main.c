#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "../jsonFile.h"
#include "../packageJsonFile.h"

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
	char *nw_ver = json_file_get_value_from_key(&f, "nw", deps + 1);
	int result = strcmp(version, "1.0.0") == 0 && strcmp(nw_ver, "^0.12.2") == 0;
	json_file_free(&f);
	free(version);
	free(nw_ver);
	return result;
}

int parseAppPackageJson(){
	packageJsonFile_t packageJson;
	int result = packageJson_file_parse("package.json", &packageJson);
	if(result != JSON_SUCCESS){
		packageJson_file_free(&packageJson);
		return 0;
	}
	result = strcmp(packageJson.name, "nw-image-viewer") == 0 && strcmp(packageJson.versionFilter, ">=0.12.2") == 0 && packageJson.forceLatest;
	packageJson_file_free(&packageJson);
	return result;
}

int main(int argc, char **argv){
	test_init(3);
	test_add("Parse a simple JSON file", parseSimpleJson);
	test_add("Parse a package.json file", parsePackageJson);
	test_add("Cast an application's package.json file to a packageJsonFile_t struct", parseAppPackageJson);
	return test_run();
}
