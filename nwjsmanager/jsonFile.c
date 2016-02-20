#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jsmn.h>
#include "jsonFile.h"

//Loads a text file into a buffer string, which is returned. 
static char* readFile(char *path){
	FILE *f = fopen(path, "r");
	if(!f)
		return NULL;
	fseek(f, 0, SEEK_END);
	char *buffer = malloc(ftell(f)*sizeof(char));
	fseek(f, 0, SEEK_SET);
	int i = 0;
	while(!feof(f))
		buffer[i++] = fgetc(f);
	buffer[i - 1] = '\0';
	fclose(f);
	return buffer;
}

static int is_jsmn_error(int i){
	return i == JSMN_ERROR_INVAL || i == JSMN_ERROR_NOMEM || i == JSMN_ERROR_PART;
}

//Reads a json file and parses it using the jsmn library.
int json_file_parse(char* file, jsonFile_t *out){
	char* buf = readFile(file);
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
		free(f->str);
		free(f->tokens);
		f->tokenCount = 0;
	}
}

//Get the string value of a token.
char* json_file_get_token_value(jsonFile_t *f, int index){
	jsmntok_t token = f->tokens[index];
	int len = token.end - token.start;
	char *result = malloc((len + 1) * sizeof(char));
	sprintf(result, "%.*s", len, f->str + token.start);
	return result;
}

//Find for a token, direct children of root (last argument) with the string value equal to key (second argument)
int json_file_get_token_index(jsonFile_t *f, char *key, int root){
	if(!f)
		return JSON_ERROR;
	int result = JSON_ERROR;
	for(int i = root; i < f->tokens[root].size; i++){
		char *val = json_file_get_token_value(f, i);
		if(strcmp(val, key) == 0)
			result = i;
		free(val);
		if(result != JSON_ERROR)
			break;
		if(i != root && i < f->tokenCount - 1)
			i += f->tokens[i].size; //to skip nested tokens
	}
	return result;
}
