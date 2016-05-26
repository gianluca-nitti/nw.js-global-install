#ifndef _UPDATE_H
#define _UPDATE_H
#include <stdbool.h>
#include "indexJsonFile.h"

#define NWJSMANAGER_VERSION "1.0.0" //current version
#define UPDATE_URL "http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/v%s/nwjsmanager%s"

bool update_required(indexJsonFile_t *indexJson);
bool update_install(indexJsonFile_t *indexJson, int (*downloadCallback)(long total, long now, double kBps), int argc, char **argv);

#endif
