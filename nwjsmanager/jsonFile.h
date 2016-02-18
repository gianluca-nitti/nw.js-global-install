#ifndef _PACKAGEJSONPARSER_H
#define _PACKAGEJSONPARSER

#include <jsmn.h>

typedef struct jsonToken{
	jsmntype_t type;
	char* val;
	struct jsonToken* children;
}jsonToken_t;

jsonToken_t* json_file_parse(char* file);

#endif
