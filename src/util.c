#include <stdint.h>
#include "tl_common.h"

unsigned int strlen(const char *str) {

    unsigned int len = 0;

    if (str != NULL) {
        while (*str++) {

            len++;

        }
    }

    return len;
}
