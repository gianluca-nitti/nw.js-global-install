#include <stdlib.h>
#include <string.h>

char *string_concat(char *str1, char *str2){
	char *result = malloc((strlen(str1) + strlen(str2) + 1)*sizeof(char));
	strcpy(result, str1);
	return strcat(result, str2);
}
