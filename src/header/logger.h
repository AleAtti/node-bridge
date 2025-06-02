#ifndef LOGGER_H
#define LOGGER_H

#define logger_log(level, format, ...) \
    logger_log_internal((level), format, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

typedef enum
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

typedef enum
{
    LOG_OUTPUT_NONE = 0,
    LOG_OUTPUT_CONSOLE = 1 << 0,
    LOG_OUTPUT_FILE = 1 << 1,
    LOGG_OUTPUT_BOTH = LOG_OUTPUT_CONSOLE | LOG_OUTPUT_FILE // 0001 | 0010 = 0011 
} LogOutput;


// Log entry structure to hold log messages --> will be ussed in the future for log file management
typedef struct
{
    LogLevel level;
    const char *message;
} LogEntry;

typedef struct
{
    const char *log_dir;
    int max_age_days; // Maximum age of log files in days
    LogLevel min_level; // Minimum log level to record
    LogOutput output; // Output options for logging
} LoggerConfig;

void logger_init(const LoggerConfig *config);
void logger_cleanup(void);
void logger_log_internal(LogLevel level, const char *format, const char *file, int line, const char *func, ...);

#endif