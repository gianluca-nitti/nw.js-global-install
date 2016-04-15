#ifndef _EXTRACTARCHIVE_H
#define _EXTRACTARCHIVE_H

#define ARCHIVE_SUCCESS 0
#define ARCHIVE_FILE_ACCESS_ERROR -1

int extractArchive(char *srcFile, char *destDir);

#endif
