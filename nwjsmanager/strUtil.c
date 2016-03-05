#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "strUtil.h"

//Allocates the required memory and concatenates the n passed strings.
char *string_concat(int n, ...){
	va_list ap;
	int len = 0;
	va_start(ap, n);
	for(int i = 0; i < n; i++)
		len += strlen(va_arg(ap, const char *)) + 1;
	va_end(ap);
	if(len == 0)
		return NULL;
	char *result = malloc(len * sizeof(char));
	result[0] = '\0';
	va_start(ap, n);
	for(int i = 0; i < n; i++)
		strcat(result, va_arg(ap, const char *));
	va_end(ap);
	return result;
}
