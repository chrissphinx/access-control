#ifndef __LIB_H__
#define __LIB_H__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "string.h"

int s_exit(int);
char* get_username();
int has_access(const char*, char);

#endif
