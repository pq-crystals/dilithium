#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int read_from_base64_file(const char* filepath, const char* header, const char* footer,
                         uint8_t* output, size_t expected_size);

int write_to_base64_file(const char* filepath, const char* header, const char* footer,
                        const uint8_t* data, size_t data_len);

#endif // BASE64_H 