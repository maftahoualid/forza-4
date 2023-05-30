#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "struct.c"

errno;

getpriority(int class, int id);
setpriority(int class, int id, int niceval);
perror(const char *s);