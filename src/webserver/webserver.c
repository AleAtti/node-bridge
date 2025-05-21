#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
#include <cjson/cJSON.h>
#include "utils.h"
#include "config.h"

#include "webserver.h"
static const char *ok_json = "{\"status\":\"ok\"}";

static const char *get_mime_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
        return "application/octet-stream";
    if (strcmp(ext, ".html") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".json") == 0)
        return "application/json";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    return "application/octet-stream";
}

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
        static char *post_data = NULL;
        if (*con_cls == NULL)
        {
            // First call → allocate
            *con_cls = (void *)1; // Just a marker
            post_data = calloc(1, 1);
            return MHD_YES;
        }

        if (*upload_data_size > 0)
        {
            // Accumulate incoming data
            post_data = realloc(post_data, strlen(post_data) + *upload_data_size + 1);
            strncat(post_data, upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }

        // Final call — save to file
        FILE *fp = fopen("config.json", "w");
        if (fp)
        {
            fwrite(post_data, 1, strlen(post_data), fp);
            fclose(fp);
        }

        // Optional: reload and validate
        Config cfg = load_config("config.json");
        printf("[WebServer] Config reloaded: COM %d, HID %d, Hostname %s\n",
               cfg.use_com, cfg.use_hid, cfg.hostname);

        free(post_data);
        post_data = NULL;
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
    getchar(); // Wait for enter key to stop
    MHD_stop_daemon(daemon);

    return 0;
}
