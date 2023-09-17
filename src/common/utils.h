#include <string.h>

#define MEMCLEAR(dest) explicit_bzero(&dest, sizeof(dest));
