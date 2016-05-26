#include <curl/curl.h>
#include "download.h"

typedef struct{
	CURL* curl;
	int (*callback)(long total, long now, double kBps);
}infoCbData_t;



static size_t writeCb(void *ptr, size_t size, size_t nmemb, FILE *f){
	return fwrite(ptr, size, nmemb, f);
}

static int infoCb(infoCbData_t *data, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow){
	double speed = 0;
	if(dltotal != 0 && curl_easy_getinfo(data->curl, CURLINFO_SPEED_DOWNLOAD, &speed) == CURLE_OK)
		return data->callback((long)dltotal, (long)dlnow, speed/1000);
	return 0;
}

//Starts the HTTP download form url to file. Calls the progressCb() callback function to report progress.
int download(char *url, char *file, int (*progressCb)(long total, long now, double kBps)){
	FILE *f = fopen(file, "wb");
	if(!f)
		return DOWNLOAD_ERROR_FILE;
	CURL *curl = curl_easy_init();
	if(!curl){
		fclose(f);
		return DOWNLOAD_ERROR_CURL;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, infoCb);
	infoCbData_t data = {curl, progressCb};
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &data);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //debug
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1); //fail on server errors (e.g. 404, 500, ecc.)
	int result = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(f);
	if(result == CURLE_OK)
		return DOWNLOAD_SUCCESS;
	return result;
}
