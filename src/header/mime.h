/*
 * mime.h
 * This file contains the function declarations for handling MIME types.
 * It includes a function to get the MIME type based on the file extension.
 */

#ifndef MIME_H
#define MIME_H

/*
 * Function to get the MIME type based on the file extension.
 * Input:   A file path.
 * Output:  The MIME type as a string.
 */
const char *get_mime_type(const char *filename);
#endif