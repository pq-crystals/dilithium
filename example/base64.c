#include "base64.h"
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define MAX_LINE_LENGTH 1024
#define MAX_BASE64_LENGTH 8192

// Add at the top, after includes
static int debug_output = 0;

int read_from_base64_file(const char* filepath, const char* header, const char* footer,
                         uint8_t* output, size_t expected_size) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return -1;
    }

    // Skip header
    char line[MAX_LINE_LENGTH];
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Failed to read header\n");
        fclose(file);
        return -3;
    }

    // Read base64 content into a temporary buffer
    char base64_data[MAX_BASE64_LENGTH] = {0};
    char temp_line[MAX_LINE_LENGTH];
    size_t total_len = 0;

    // Read all lines until footer
    while (fgets(temp_line, sizeof(temp_line), file)) {
        if (strstr(temp_line, footer) != NULL) {
            break;
        }
        // Remove trailing newline if present
        size_t line_len = strlen(temp_line);
        if (line_len > 0 && temp_line[line_len-1] == '\n') {
            temp_line[line_len-1] = '\0';
            line_len--;
        }
        // Append to base64_data
        if (total_len + line_len < sizeof(base64_data)) {
            strcat(base64_data, temp_line);
            total_len += line_len;
        } else {
            fprintf(stderr, "Base64 buffer overflow! Need more than %zu bytes\n", sizeof(base64_data));
            fclose(file);
            return -5;
        }
    }

    // Modify debug output to use debug flag
    if (debug_output) {
        fprintf(stderr, "Base64 data length: %zu\n", strlen(base64_data));
    }

    // Setup BIO chain for base64 decoding
    BIO* bio_mem = BIO_new_mem_buf(base64_data, -1);
    BIO* bio_b64 = BIO_new(BIO_f_base64());
    BIO* bio_chain = BIO_push(bio_b64, bio_mem);
    
    // Disable newline checking
    BIO_set_flags(bio_b64, BIO_FLAGS_BASE64_NO_NL);

    // Read and decode
    size_t total_bytes = BIO_read(bio_chain, output, expected_size);

    // Modify debug output to use debug flag
    if (debug_output) {
        fprintf(stderr, "Decoded %zu bytes, expected %zu bytes\n", total_bytes, expected_size);
    }

    fclose(file);
    BIO_free_all(bio_chain);

    return (total_bytes == expected_size) ? 0 : -4;
}

int write_to_base64_file(const char* filepath, const char* header, const char* footer,
                        const uint8_t* data, size_t data_len) {
    FILE* file = fopen(filepath, "w");
    if (!file) {
        return -1;
    }

    // Setup BIO chain for base64 encoding
    BIO* bio_b64 = BIO_new(BIO_f_base64());
    BIO* bio_mem = BIO_new(BIO_s_mem());
    BIO* bio_chain = BIO_push(bio_b64, bio_mem);
    BUF_MEM* buffer_ptr;

    // Write data and flush
    BIO_write(bio_chain, data, data_len);
    BIO_flush(bio_chain);
    BIO_get_mem_ptr(bio_chain, &buffer_ptr);

    // Write to file with headers
    fprintf(file, "%s\n", header);
    fprintf(file, "%.*s", (int)buffer_ptr->length, buffer_ptr->data);
    fprintf(file, "%s\n", footer);

    BIO_free_all(bio_chain);
    fclose(file);
    return 0;
}