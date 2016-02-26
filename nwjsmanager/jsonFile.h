#ifndef _JSONFILE_H
#define _JSONFILE_H

#include <stdbool.h>
#include <jsmn.h>

#define JSON_ERROR -1
#define JSON_SUCCESS 0

typedef struct{
	char *str;
	int tokenCount;
	jsmntok_t *tokens;
}jsonFile_t;

int json_file_parse(char* file, jsonFile_t *out);
void json_file_free(jsonFile_t *f);
char* json_file_get_token_value(jsonFile_t *f, int index);
int json_file_get_token_index(jsonFile_t *f, char *key, int root);
int json_file_get_subtoken_abs_index(jsonFile_t *f, int root, int n);
char *json_file_get_value_from_key(jsonFile_t *f, char *key, int root);
bool json_file_token_is_boolean(jsonFile_t *f, int index);
bool json_file_get_token_value_boolean(jsonFile_t *f, int index);
int* json_file_get_subtokens_indices(jsonFile_t *f, int root);

#endif
