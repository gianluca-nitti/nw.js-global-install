#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "../jsonFile.h"

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
	int result = strcmp(val, "key2") == 0;
	free(val);
	return result;
}

int parsePackageJson(){
	jsonFile_t f;
	if(json_file_parse("package.json", &f) == 0)
		return 1;
	//TODO
	return 0;
}

int main(int argc, char **argv){
	test_init(2);
	test_add("Parse a simple JSON file", parseSimpleJson);
	test_add("Parse a package.json file", parsePackageJson);
	return test_run();
}
