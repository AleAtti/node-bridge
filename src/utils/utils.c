#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char* read_file(const char* path, size_t* size) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(*size + 1);
    if (!buffer) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *size, file);
    buffer[*size] = '\0'; // Null-terminate the string

    fclose(file);
    return buffer;
}