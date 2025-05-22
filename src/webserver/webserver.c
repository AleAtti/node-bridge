/*
Author:           Alexander Attisani
Description:      A small webserver to serve the web interface and handle configuration

Resources:  https://www.gnu.org/software/libmicrohttpd/manual/libmicrohttpd.html#index-MHD_005fstart_005fdaemon
            https://www.gnu.org/software/libmicrohttpd/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <cjson/cJSON.h>
#include "utils.h"
#include "config.h"
#include "mime.h"

#include "webserver.h"
static const char *ok_json = "{\"status\":\"ok\"}";

typedef struct
{
    char *data;
    size_t size;
} PostData;

static enum MHD_Result answer_to_connection(void *cls,
                                            struct MHD_Connection *connection,
                                            const char *url,
                                            const char *method,
                                            const char *version,
                                            const char *upload_data,
                                            size_t *upload_data_size,
                                            void **con_cls)
{

    // Handle GET /getConfig
    if (strcmp(url, "/getConfig") == 0 && strcmp(method, "GET") == 0)
    {
        size_t size;
        char *data = read_file("./config.json", &size);
        if (!data)
            data = strdup("{}");

        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(data), data, MHD_RESPMEM_MUST_FREE);
        MHD_add_response_header(response, "Content-Type", "application/json");
        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    // Handle POST /setConfig
    if (strcmp(url, "/setConfig") == 0 && strcmp(method, "POST") == 0)
    {
        if (*con_cls == NULL)
        {
            PostData *pd = calloc(1, sizeof(PostData));
            *con_cls = pd;
            return MHD_YES;
        }

        PostData *pd = *con_cls;

        if (*upload_data_size > 0)
        {
            pd->data = realloc(pd->data, pd->size + *upload_data_size + 1); // +1 for null-termination
            memcpy(pd->data + pd->size, upload_data, *upload_data_size);
            pd->size += *upload_data_size;
            pd->data[pd->size] = '\0'; // Null-terminieren für JSON-Kompatibilität
            *upload_data_size = 0;
            return MHD_YES;
        }

        save_file("config.json", pd->data, pd->size);

        // Optional: reload and validate
        Config cfg = load_config("config.json");
        printf("[WebServer] Config reloaded: COM %d, HID %d, Hostname %s\n",
               cfg.General.use_com, cfg.General.use_hid, cfg.General.hostname);

        free(pd->data);
        free(pd);
        *con_cls = NULL;

        // Respond to JS

        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(ok_json), (void *)ok_json, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(response, "Content-Type", "application/json");
        return MHD_queue_response(connection, MHD_HTTP_OK, response);
    }

    // Ignore favicon requests
    if (strcmp(url, "/favicon.ico") == 0)
    {
        struct MHD_Response *response = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NO_CONTENT, response);
    }

    // Map requested URL to file path
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "www%s", strcmp(url, "/") == 0 ? "/index.html" : url);

    // Try to load file
    size_t size;
    char *data = read_file(filepath, &size);
    if (!data)
    {
        const char *not_found = "404 Not Found";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(not_found), (void *)not_found, MHD_RESPMEM_PERSISTENT);
        return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    }

    // Determine MIME type
    const char *mime = get_mime_type(filepath);
    // Serve file
    struct MHD_Response *response = MHD_create_response_from_buffer(size, data, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", mime);
    return MHD_queue_response(connection, MHD_HTTP_OK, response);
}

int start_webserver(int server_port)
{
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, server_port, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    printf("Server running on http://localhost:%d\n", server_port);
    pause(); // Wait for CTRl+C to exit

    MHD_stop_daemon(daemon);

    return 0;
}
