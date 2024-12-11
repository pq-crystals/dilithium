#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stddef.h>

void debug_hexa(const char* name, const uint8_t* array, size_t length) {
    printf("%s length: %zu bytes\n", name, length);
    printf("%s hex dump: ", name);
    for (size_t i = 0; i < length; i++) {
        printf("%02x", array[i]);
        if ((i + 1) % 32 == 0 && i + 1 < length) {
            printf("\n             ");
        }
    }
    printf("\n");
}

#endif 