#ifndef _UPDATE_H
#define _UPDATE_H
#include <stdbool.h>

#define NWJSMANAGER_VERSION "1.0.0" //current version
#define UPDATE_URL "http://github.com/gianluca-nitti/nw.js-global-install/releases/download/v%s/nwjsmanager%s"

bool update_required(indexJsonFile_t *indexJson);
bool update_install(indexJsonFile_t *indexJson, int (*downloadCallback)(long total, long now, double kBps));

#endif
