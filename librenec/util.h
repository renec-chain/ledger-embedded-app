#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))
#define BAIL_IF(x)           \
    do {                     \
        int err = x;         \
        if (err) return err; \
    } while (0)
#define MIN(a, b) ((a) < (b) ? (a) : (b));

void explicit_bzero(void *s, size_t n);

#define assert_string_equal(actual, expected) assert(strcmp(actual, expected) == 0)

#define assert_pubkey_equal(actual, expected) assert(memcmp(actual, expected, 32) == 0)

#ifndef UNUSED
#define UNUSED(x) (void) x
#endif

#endif  // UTIL_H
