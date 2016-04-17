#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "strUtil.h"
#ifndef _WIN32
	#include "linux-only/untar.h"
#endif
#include "extractArchive.h"

//Extracts a compressed archive (works with .tar.gz under Linux and with .zip on Windows)
int extractArchive(char *srcFile, char *destDir){
	#ifdef _WIN32
		return 0; //TODO
	#else
		gzFile tarGzFile = gzopen(srcFile, "rb");
		if(!tarGzFile)
			return ARCHIVE_FILE_ACCESS_ERROR;
		char* tarFilePath = string_concat(2, destDir, ".tar");
		FILE* tarFile = fopen(tarFilePath, "wb");
		if(!tarFile){
			free(tarFilePath);
			gzclose(tarGzFile);
			return ARCHIVE_FILE_ACCESS_ERROR;
		}
		int readByte = 0;
		while((readByte = gzgetc(tarGzFile)) != -1)
			fputc(readByte, tarFile);
		fclose(tarFile);
		gzclose(tarGzFile);
		extractTar(tarFilePath, destDir); //TODO:check for errors
		free(tarFilePath);
		return ARCHIVE_SUCCESS; //TODO
	#endif
}
