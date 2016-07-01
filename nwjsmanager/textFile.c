#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "textFile.h"

//Loads a text file into a buffer string, which is returned.
char* readTextFile(char *path){
	FILE *f = fopen(path, "r");
	if(!f)
		return NULL;
	//The following if statements are to ensure the file is not a directory.
	if(fseek(f, 0, SEEK_END) < 0){
		fclose(f);
		return NULL;
	}
	long len = ftell(f);
	if(len == -1L || len == LONG_MAX){
		fclose(f);
		return NULL;
	}
	char *buffer = malloc((len + 1)*sizeof(char));
	fseek(f, 0, SEEK_SET);
	int i = 0;
	while(!feof(f))
		buffer[i++] = fgetc(f);
	buffer[i - 1] = '\0';
	fclose(f);
	return buffer;
}

//Writes a string to a text file (replaces the file, doesn't append).
int writeTextFile(char *path, char *data){
	FILE *f = fopen(path, "w");
	if(!f)
		return 0;
	int res = fputs(data, f) != EOF;
	fclose(f);
	return res;
}
