/* * utils.h
 * This file contains utility function declarations for string manipulation and file reading.
 * Function to convert strings to lowercase, trim whitespace, and read files.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>  

/*
 * Function to assert a condition and print an error message if the condition is false.
 * Input:   A condition, a message, function name, file name, and line number.
 * Output:  Exits the program if the condition is false.
 */
#define ASSERT_MSG "Assertion failed: (%s), function %s, file %s, line %d\n"
#define assert(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, ASSERT_MSG, #expr, __func__, __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
/*
 * Function to assert a condition and return a value if the condition is false.
 * Input:   A condition, a message, function name, file name, line number, and return value.
 * Output:  Returns the specified value if the condition is false.
 */
#define assert_return(expr, return_value) do { \
    if (!(expr) || (expr) == (return_value)) { \
        fprintf(stderr, ASSERT_MSG, #expr, __func__, __FILE__, __LINE__); \
        return return_value; \
    } \
} while (0)

/* * Function to convert a string to lowercase.
 * Input:   A string.
 * Output:  A new string with all characters in lowercase.
 */
char *toLowerCase(const char *str);

/*
 * Function to trim whitespace from the beginning and end of a string.
 * Input:   A string.
 * Output:  A new string with leading and trailing whitespace removed.
 */
char *trim(char *str);

/*
 * Function to read a file into a string.
 * Input:   A file path and a pointer to store the size of the file.
 * Output:  A string containing the file contents and the size of the file.
 */
char *read_file(const char *file_path, size_t *size);

/*
 * Function to save a string to a file.
 * Input:   A file path and a string to save.
 * Output:  Returns 0 on success, -1 on failure.
 */
int save_to_file(const char* path, const char* data);

int save_file(const char* path, const char* data, size_t size);

char hid_to_ascii(uint8_t c, int shift);

#endif