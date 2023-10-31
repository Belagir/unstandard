
#include <ustd/common.h>
#include <ustd/logging.h>

#include <stdarg.h>

#define LOGGER_MAXIMUM_NUMBER 8u

typedef struct logger {
    FILE *target_stream;
    logger_on_destroy on_destroy;
    allocator alloc;
} logger;

// -------------------------------------------------------------------------------------------------
static struct {
    logger loggers[LOGGER_MAXIMUM_NUMBER];
    size_t free_logger_index;
} logger_module = { 0u };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const char *logger_severity_msg[LOGGER_SEVERITIES_NUMBER] = {
        // LOGGER_NONE
        "",
        // LOGGER_INFO
        "[INFO] ",
        // LOGGER_WARN
        "[WARNING] ",
        // LOGGER_ERRO
        "[ERROR] ",
        // LOGGER_CRIT
        "[CRITICAL] ",
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
logger * logger_create(allocator alloc, FILE target[static 1], logger_on_destroy on_destroy)
{
    logger *new_logger = nullptr;

    if (logger_module.free_logger_index >= LOGGER_MAXIMUM_NUMBER) {
        return nullptr;
    }

    new_logger = logger_module.loggers + logger_module.free_logger_index;
    *new_logger = (logger) { .alloc = alloc, .target_stream = target, .on_destroy = on_destroy };
    logger_module.free_logger_index += 1u;

    return new_logger;
}

// -------------------------------------------------------------------------------------------------
void logger_destroy(logger **custom_logger)
{
    if ((!custom_logger) || (!*custom_logger)) {
        return;
    }

    if (((*custom_logger)->target_stream) && ((*custom_logger)->on_destroy == LOGGER_ON_DESTROY_CLOSE_STREAM)) {
        fclose((*custom_logger)->target_stream);
    }

    if (logger_module.free_logger_index > 0u) {
        logger_module.free_logger_index -= 1u;
        bytewise_copy((*custom_logger), logger_module.loggers + logger_module.free_logger_index, sizeof(**custom_logger));
    }

    (*custom_logger) = nullptr;
}

// -------------------------------------------------------------------------------------------------
void
logger_log(logger *logger, logger_severity severity, char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    if ((!logger) || (!logger->target_stream)) {
        return;
    }

    fprintf(logger->target_stream, logger_severity_msg[severity]);
    vfprintf(logger->target_stream, msg, args);

    va_end(args);
}
