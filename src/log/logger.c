
#include <ustd/logging.h>

#include <stdarg.h>

#include <ustd/common.h>

#define CUSTOM_LOGGER_MAXIMUM_NUMBER 8u

typedef struct custom_logger_t
{
    FILE *target_stream;
    custom_logger_on_destroy_t on_destroy;
} custom_logger_t;

// -------------------------------------------------------------------------------------------------
static struct
{
    custom_logger_t loggers[CUSTOM_LOGGER_MAXIMUM_NUMBER];
    size_t free_logger_index;
} logger_module = { 0u };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const char *custom_logger_severity_msg[CUSTOM_LOGGER_SEVERITIES_NUMBER] =
{
        // CUSTOM_LOGGER_NONE
        "",
        // CUSTOM_LOGGER_INFO
        "[INFO] ",
        // CUSTOM_LOGGER_WARN
        "[WARNING] ",
        // CUSTOM_LOGGER_ERRO
        "[ERROR] ",
        // CUSTOM_LOGGER_CRIT
        "[CRITICAL] ",
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
custom_logger_t *
custom_logger_create(FILE *target, custom_logger_on_destroy_t on_destroy)
{
    custom_logger_t *new_logger = NULL;

    if (logger_module.free_logger_index >= CUSTOM_LOGGER_MAXIMUM_NUMBER)
    {
        return NULL;
    }

    new_logger = logger_module.loggers + logger_module.free_logger_index;
    logger_module.free_logger_index += 1u;

    new_logger->target_stream = target;
    new_logger->on_destroy = on_destroy;

    return new_logger;
}

// -------------------------------------------------------------------------------------------------
void
custom_logger_destroy(custom_logger_t **custom_logger)
{
    if ((!custom_logger) || (!*custom_logger))
    {
        return;
    }

    if (((*custom_logger)->target_stream) && ((*custom_logger)->on_destroy == CUSTOM_LOGGER_ON_DESTROY_CLOSE_STREAM))
    {
        fclose((*custom_logger)->target_stream);
    }

    if (logger_module.free_logger_index > 0u)
    {
        logger_module.free_logger_index -= 1u;
        bytewise_copy((*custom_logger), logger_module.loggers + logger_module.free_logger_index, sizeof(**custom_logger));
    }


    (*custom_logger) = NULL;
}

// -------------------------------------------------------------------------------------------------
void
custom_logger_log(custom_logger_t *logger, custom_logger_severity_t severity, char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    if ((!logger) || (!logger->target_stream)) {
        return;
    }

    fprintf(logger->target_stream, custom_logger_severity_msg[severity]);
    vfprintf(logger->target_stream, msg, args);

    va_end(args);
}
