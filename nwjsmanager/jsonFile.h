#ifndef _PACKAGEJSONPARSER_H
#define _PACKAGEJSONPARSER

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

#endif
