#ifndef _UTILS_H
#define _UTILS_H

int recursiveMkdir(char *pathname, int mode);

#ifndef _WIN32
void recursiveChmod(char *path, mode_t mode);
#endif

#endif
