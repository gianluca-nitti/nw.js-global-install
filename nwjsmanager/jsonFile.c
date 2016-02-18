#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jsmn.h>
#include "jsonFile.h"

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

static jsonToken_t* buildTokenTree(jsmntok_t* tokens, int start, int end, char *jsonStr){
	int rootTokenCount = 0;
	int strPos = 0;
	for(int i = start; i < end; i++)
		if(tokens[i].start >= strPos){
			rootTokenCount++;
			strPos = tokens[i].end;
		}
	jsonToken_t* rootTokens = malloc(rootTokenCount * sizeof(jsonToken_t));
	strPos = 0;
	for(int i = start; i < end; i++)
		if(tokens[i].start >= strPos){
			int tokenLen = tokens[i].end - tokens[i].start;
			char *val = malloc((tokenLen + 1) * sizeof(char));
			sprintf(val, "%.*s", tokenLen, jsonStr + tokens[i].start);
			jsonToken_t* children = NULL;
			if(tokens[i].type == JSMN_OBJECT)
				children = buildTokenTree(tokens, i, i + tokens[i].size, jsonStr);
			int rootPos = i - start;
			rootTokens[rootPos].type = tokens[i].type;
			rootTokens[rootPos].val = val;
			rootTokens[rootPos].children = children;
			strPos = tokens[i].end;
		}
	return rootTokens;
}

//Reads a json file and parses it using the jsmn library.
jsonToken_t* json_file_parse(char* file){
	char* buf = readFile(file);
	if(!buf)
		return NULL;
	jsmn_parser parser;
	jsmn_init(&parser);
	int nTokens = jsmn_parse(&parser, buf, strlen(buf), NULL, 0);
	if(is_jsmn_error(nTokens)){
		free(buf);
		return NULL;
	}
	jsmntok_t tokens[nTokens];
	int result = jsmn_parse(&parser, buf, strlen(buf), tokens, nTokens);
	if(is_jsmn_error(result)){
		free(buf);
		return NULL;
	}
	jsonToken_t* jsonTokens = buildTokenTree(tokens, 0, nTokens, buf);
	free(buf);
	return jsonTokens;
}

//TODO: json_file_free()
