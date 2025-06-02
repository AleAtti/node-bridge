#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"

static LoggerConfig logger_config = {0};

static const char *log_level_to_string(LogLevel level)
{
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    case LOG_LEVEL_INFO:
        return "INFO";
    case LOG_LEVEL_WARNING:
        return "WARNING";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    case LOG_LEVEL_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

static void logger_dir_exists()
{
    struct stat st = {0};
    if (stat(logger_config.log_dir, &st) == -1)
    {
        if (mkdir(logger_config.log_dir, 0700) != 0)
        {
            fprintf(stderr, "Failed to create log directory: %s\n", logger_config.log_dir);
            exit(EXIT_FAILURE);
        }
    }
}

static const char *get_log_file_path()
{
    static char log_file_path[256];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char date_part[64];

    strftime(date_part, sizeof(date_part), "%Y-%m-%d.log", tm_info);
    snprintf(log_file_path, sizeof(log_file_path), "%s/%s", logger_config.log_dir, date_part);

    return log_file_path;
}

static const char* get_ansi_color_for_level(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG:   return "\033[90m";  // light gray
        case LOG_LEVEL_INFO:    return "\033[0m";   // reset (default color)
        case LOG_LEVEL_WARNING: return "\033[33m";  // yellow
        case LOG_LEVEL_ERROR:   return "\033[31m";  // red
        case LOG_LEVEL_FATAL:   return "\033[1;31m";// red + bold
        default:                return "\033[0m";   // reset
    }
}

void logger_init(const LoggerConfig *config)
{
    if (!config || !config->log_dir || config->max_age_days < 0 ||
        config->min_level < LOG_LEVEL_DEBUG || config->min_level > LOG_LEVEL_FATAL)
    {
        fprintf(stderr, "Invalid logger configuration\n");
        exit(EXIT_FAILURE);
    }

    logger_config = *config;
    logger_dir_exists();
}

void logger_cleanup(void)
{
    DIR *dir = opendir(logger_config.log_dir);
    if (!dir)
        return;

    time_t now = time(NULL);
    struct dirent *entry;

    while ((entry = readdir(dir)))
    {
        if (entry->d_type != DT_REG)
            continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", logger_config.log_dir, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0)
        {
            double age_days = difftime(now, st.st_mtime) / (60 * 60 * 24);
            if (age_days > logger_config.max_age_days)
            {
                remove(path);
            }
        }
    }

    closedir(dir);
}

void logger_log_internal(LogLevel level, const char *format, const char *file, int line, const char *func, ...)
{
    if (level < logger_config.min_level)
        return;

    // Get current time and format it as a timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *tm_info = localtime(&tv.tv_sec);

    char timestamp[64];
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tv.tv_usec / 1000);

    // Prepare the log message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Construct the full log line
    char full_line[1280];
    snprintf(full_line, sizeof(full_line), "[%s] [%s] [%s] [%s] [%d]: %s",
             timestamp, log_level_to_string(level), file, func, line, message);

    // Output the log message to console
    if (logger_config.output & LOG_OUTPUT_CONSOLE) {
        const char *ansi_color = get_ansi_color_for_level(level);
        const char *reset_color = "\033[0m";
        fprintf(stderr, "%s%s%s\n", ansi_color, full_line, reset_color);
    }

    // If file output is enabled, write to the log file
    if (logger_config.output & LOG_OUTPUT_FILE) {
        FILE* f = fopen(get_log_file_path(), "a");
        if (f) {
            fprintf(f, "%s\n", full_line);
            fclose(f);
        } else {
            fprintf(stderr, "Logger: failed to open log file.\n");
        }
    }
}

