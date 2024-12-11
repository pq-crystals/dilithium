#include <stdio.h>
#include <string.h>
#include "base64.h"

#define TEST_DATA_SIZE 80

int main() {
    // Test data
    uint8_t original_data[TEST_DATA_SIZE];
    uint8_t decoded_data[TEST_DATA_SIZE];
    
    // Initialize test data
    for (int i = 0; i < TEST_DATA_SIZE; i++) {
        original_data[i] = i;
    }

    const char* header = "-----BEGIN TEST DATA-----";
    const char* footer = "-----END TEST DATA-----";

    // Test writing
    printf("Writing test data to file...\n");
    int write_result = write_to_base64_file("test.b64", header, footer, 
                                          original_data, TEST_DATA_SIZE);
    if (write_result != 0) {
        printf("Error writing base64 file: %d\n", write_result);
        return 1;
    }

    // Test reading
    printf("Reading test data from file...\n");
    int read_result = read_from_base64_file("test.b64", header, footer,
                                          decoded_data, TEST_DATA_SIZE);
    if (read_result != 0) {
        printf("Error reading base64 file: %d\n", read_result);
        // Enhanced debug information
        FILE *fp = fopen("test.b64", "r");
        if (fp) {
            char buffer[256];
            printf("File contents (with hex for newlines):\n");
            while (fgets(buffer, sizeof(buffer), fp)) {
                printf("Line: ");
                for (size_t i = 0; buffer[i]; i++) {
                    if (buffer[i] == '\n') {
                        printf("\\n[0x%02X]", buffer[i]);
                    } else {
                        printf("%c", buffer[i]);
                    }
                }
                printf("\n");
            }
            fclose(fp);
        }
        return 1;
    }

    // Compare results
    if (memcmp(original_data, decoded_data, TEST_DATA_SIZE) == 0) {
        printf("Test passed! Data matches after encode/decode cycle\n");
    } else {
        printf("Test failed! Data mismatch after encode/decode cycle\n");
        return 1;
    }

    return 0;
}