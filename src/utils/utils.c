#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "utils.h"

char *toLowerCase(const char *str) {
    char *lower_str = malloc(strlen(str) + 1);

    assert_return (lower_str, NULL);

    // Convert each character to lowercase
    for (size_t i = 0; str[i]; i++) {
        lower_str[i] = tolower((unsigned char)str[i]);
    }
    lower_str[strlen(str)] = '\0'; // Null-terminate the string

    return lower_str;
}

char* trim(char* str){
    assert_return (str, NULL);
    while(isspace((unsigned char)*str)){
        str++;
    }
    char* end = str + strlen(str) -1;
    while(end>str && isspace((unsigned char) *end)){
        end--;
    }
    end[1] = '\0';
    return str;
}

char* read_file(const char* path, size_t* size) {
    FILE* file = fopen(path, "rb");
    // assert(file);
    assert_return (file, NULL);
    // if (!file) {
    //     fprintf(stderr, "[read_file] Could not open file: %s\n", path);
    //     return NULL;
    // }

    // Move the file pointer to the end to get the size
    fseek(file, 0, SEEK_END);
    // Get the size of the file
    *size = ftell(file);
    // Check if the size is valid
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(*size + 1);
    if (!buffer) {
        assert(buffer);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, *size, file);
    buffer[*size] = '\0'; // Null-terminate the string

    fclose(file);
    return buffer;
}

int save_to_file(const char* path, const char* data) {
    FILE* file = fopen(path, "w");
    assert_return (file, -1);
    // Write the data to the file
    size_t written = fwrite(data, sizeof(char), strlen(data), file);
    // Check if the write was successful
    if (written != strlen(data)) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

int save_file(const char* path, const char* data, size_t size) {
    FILE* file = fopen(path, "w");
    assert_return (file, -1);
    // Write the data to the file
    size_t written = fwrite(data, sizeof(char), size, file);
    // Check if the write was successful
    if (written != size) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

char hid_to_ascii(uint8_t code, int shift)
{
    // HID usage IDs → ASCII mapping (US QWERTY layout)
    const char ascii[] = {
        0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
        '3', '4', '5', '6', '7', '8', '9', '0', '\n', 0, 0, '\t', ' ', '-', '=',
        '[', ']', '\\', '#', ';', '\'', '`', ',', '.', '/', 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    const char shift_ascii[] = {
        0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '!', '@',
        '#', '$', '%', '^', '&', '*', '(', ')', '\n', 0, 0, '\t', ' ', '_', '+',
        '{', '}', '|', '~', ':', '"', '~', '<', '>', '?', 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (code < sizeof(ascii))
        return shift ? shift_ascii[code] : ascii[code];

    return 0;
}