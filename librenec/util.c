#include "util.h"

void explicit_bzero(void *s, size_t n) {
    volatile unsigned char *p = s;
    while (n--) *p++ = 0;
}
