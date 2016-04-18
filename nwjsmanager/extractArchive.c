#include <stdio.h>
#include <stdlib.h>
#include "strUtil.h"
#ifdef _WIN32
	#include "win-only/unzip.h"
#else
	#include <zlib.h>
	#include "linux-only/untar.h"
#endif
#include "extractArchive.h"

//Extracts a compressed archive (works with .tar.gz under Linux and with .zip on Windows)
int extractArchive(char *srcFile, char *destDir){
	#ifdef _WIN32
		return extractZip(srcFile, destDir);
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
		int exitStatus = extractTar(tarFilePath, destDir);
		free(tarFilePath);
		if(exitStatus == TAR_SUCCESS)
			return ARCHIVE_SUCCESS;
		else
			return exitStatus;
	#endif
}
