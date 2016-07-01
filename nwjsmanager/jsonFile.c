#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jsmn.h>
#include "textFile.h"
#include "jsonFile.h"

static int is_jsmn_error(int i){
	return i == JSMN_ERROR_INVAL || i == JSMN_ERROR_NOMEM || i == JSMN_ERROR_PART;
}

//(internal function) Recursively counts all the children tokens.
static int countAllSubTokens(jsonFile_t *f, int root){
	int result = f->tokens[root].size;
	int offset = root + 1;
	for(int i = 0; i < f->tokens[root].size; i++){
		int n = countAllSubTokens(f, offset + i);
		offset += n;
		result += n;
	}
	return result;
}

//Reads a json file and parses it using the jsmn library.
int json_file_parse(char* file, jsonFile_t *out){
	char* buf = readTextFile(file);
	if(!buf)
		return JSON_ERROR;
	jsmn_parser parser;
	jsmn_init(&parser);
	int nTokens = jsmn_parse(&parser, buf, strlen(buf), NULL, 0);
	if(is_jsmn_error(nTokens)){
		free(buf);
		return nTokens;
	}
	jsmntok_t *tokens = malloc((nTokens + 1) * sizeof(jsmntok_t));
	jsmn_init(&parser);
	int result = jsmn_parse(&parser, buf, strlen(buf), tokens, nTokens);
	if(is_jsmn_error(result)){
		free(buf);
		return result;
	}
	out->str = buf;
	out->tokenCount = nTokens;
	out->tokens = tokens;
	return JSON_SUCCESS;
}

void json_file_free(jsonFile_t *f){
	if(f){
		if(f->str)
			free(f->str);
		if(f->tokens)
			free(f->tokens);
		f->tokenCount = 0;
	}
}

//Get the string value of a token.
char* json_file_get_token_value(jsonFile_t *f, int index){
	if(!f || index < 0 || index >= f->tokenCount)
		return NULL;
	jsmntok_t token = f->tokens[index];
	int len = token.end - token.start;
	char *result = malloc((len + 1) * sizeof(char));
	sprintf(result, "%.*s", len, f->str + token.start);
	return result;
}

//Search for a token, direct children of root (last argument) with the string value equal to key (second argument)
int json_file_get_token_index(jsonFile_t *f, char *key, int root){
	if(!f)
		return JSON_ERROR;
	int result = JSON_ERROR;
	int offset = root + 1;
	for(int i = 0; i < f->tokens[root].size; i++){
		char *val = json_file_get_token_value(f, offset + i);
		if(!val)
			return JSON_ERROR;
		if(strcmp(val, key) == 0)
			result = offset + i;
		free(val);
		if(result != JSON_ERROR)
			break;
		if(i < f->tokenCount - 1)
			offset += countAllSubTokens(f, offset + i); //to skip nested tokens
	}
	return result;
}

//Check if the specified token, n-th child of root, exists and return its absolute index
int json_file_get_subtoken_abs_index(jsonFile_t *f, int root, int n){
	if(!f || root < 0 || root >= f->tokenCount || n >= f->tokens[root].size)
		return JSON_ERROR;
	return root + n + 1;
}

//A slightly higher level function to get the value from a key-value pair.
char* json_file_get_value_from_key(jsonFile_t *f, char *key, int root){
	int keyIndex = json_file_get_token_index(f, key, root);
	if(keyIndex == JSON_ERROR)
		return NULL;
	int valueIndex = json_file_get_subtoken_abs_index(f, keyIndex, 0);
	if(valueIndex == JSON_ERROR)
		return NULL;
	char *value = json_file_get_token_value(f, valueIndex);
	return value;
}

//Check if the token at tokenIndex can be casted to a boolean (does NOT return it's value, only performs a check - for the value, there is the json_file_get_token_value_boolean function).
bool json_file_token_is_boolean(jsonFile_t *f, int index){
	if(!f || index < 0 || index >= f->tokenCount || f->tokens[index].type != JSMN_PRIMITIVE)
		return false;
	char *val = json_file_get_token_value(f, index);
	bool result = val[0] == 'f' || val[0] == 't';
	free(val);
	return result;
}

//Read the token's value as a boolean.
bool json_file_get_token_value_boolean(jsonFile_t *f, int index){
	if(!json_file_token_is_boolean(f, index))
		return false;
	char *val = json_file_get_token_value(f, index);
	bool result = val[0] == 't';
	free(val);
	return result;
}

//Fills a vector of integers (dinamically allocated with malloc) with the absolute indices of the tokens children of a given token in a json file.
int* json_file_get_subtokens_indices(jsonFile_t *f, int root){
	if(!f || root < 0 || root >= f->tokenCount)
		return NULL;
	int *result = malloc(f->tokens[root].size * sizeof(int));
	int offset = root + 1;
	for(int i = 0; i < f->tokens[root].size; i++){
		result[i] = offset + i;
		if(i < f->tokenCount - 1)
			offset += countAllSubTokens(f, offset + i); //to skip nested tokens
	}
	return result;
}
