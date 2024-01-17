/**
 * @file logger.h
 * @author gabriel
 * @brief Interface declaration file for creating statically-allocated custom loggers to output messages to streams.
 * This module uses an old pattern of mine where it does not need allocator to function, and will instead allocate memory directly in the global scope. While this makes its use simple, safe and compatible with a lot of systems, only a set number of loggers objects can exist at the same time.
 * @version 0.1
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

/**
 * @brief When a call is made to output some content to a stream, it can log it along a severity so it can be filtered downstream.
 *
 */
typedef enum logger_severity
{
    /// no severity should be logged along the content.
    LOGGER_SEVERITY_NONE,

    /// informational output, meant for nominal output
    LOGGER_SEVERITY_INFO,
    /// warning message, meant to notify of an invalid program state that can be recovered from
    LOGGER_SEVERITY_WARN,
    /// error message, meant to notify of an invalid program state that can not be recovered from
    LOGGER_SEVERITY_ERRO,
    /// critical error message, meant to notify of an invalid program state that is not meant to be part of the normal program flow and / or will leave some external resources in an invalid state.
    LOGGER_SEVERITY_CRIT,

    /// number of available severity options
    LOGGER_SEVERITIES_NUMBER,
} logger_severity;

/**
 * @brief Behavior when a logger is destroyed.
 *
 */
typedef enum logger_on_destroy
{
    /// do nothing more than releasing the data from memory
    LOGGER_ON_DESTROY_DO_NOTHING,
    /// in addition from releasing the data from memory, close the associated stream
    LOGGER_ON_DESTROY_CLOSE_STREAM,
} logger_on_destroy;

/**
 * @brief Opaque type to handle a statically-allocated custom logger's data.
 *
 */
typedef struct logger logger;

/**
 * @brief Creates a logger object on the static logger module memory and return a pointer to it.
 *
 * @param[in] target
 * @return logger*
 */
logger * logger_create(FILE target[static 1], logger_on_destroy on_destroy);

/**
 * @brief Release a logger object from the module memory.
 *
 * @param[inout] logger
 */
void logger_destroy(logger **logger);

/**
 * @brief Log a message associated to a severity through a custom logger.
 *
 * @param[in] logger
 * @param[in] severity
 * @param[in] msg
 * @param[in] ...
 */
void logger_log(logger *logger, logger_severity severity, char *msg, ...);

#endif
