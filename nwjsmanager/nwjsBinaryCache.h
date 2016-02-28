#ifndef _NWJSBINARYCACHE_H
#define _NWJSBINARYCACHE_H

#ifndef __SEMVER_H
#include <semver.h>
#endif

typedef struct{
	semver_t *items;
	int count;
}semverList_t;

semverList_t nwjs_binary_cache_get_versions();
void semverList_free(semverList_t *l);

#endif
