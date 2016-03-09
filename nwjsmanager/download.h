#ifndef _DOWNLOAD_H
#define _DOWNLOAD_H

#define DOWNLOAD_SUCCESS 0
#define DOWNLOAD_ERROR_FILE -1
#define DOWNLOAD_ERROR_CURL -2

int download(char *url, char *file, int (*progressCb)(long total, long now, double kBps));

#endif
