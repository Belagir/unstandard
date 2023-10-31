/**
 * @file logger.h
 * @author gabriel
 * @brief Interface declaration file for creating statically-allocated custom loggers to output messages to streams.
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
typedef enum custom_logger_severity_t
{
    /// no severity should be logged along the content.
    CUSTOM_LOGGER_SEVERITY_NONE,

    /// informational output, meant for nominal output
    CUSTOM_LOGGER_SEVERITY_INFO,
    /// warning message, meant to notify of an invalid program state that can be recovered from
    CUSTOM_LOGGER_SEVERITY_WARN,
    /// error message, meant to notify of an invalid program state that can not be recovered from
    CUSTOM_LOGGER_SEVERITY_ERRO,
    /// critical error message, meant to notify of an invalid program state that is not meant to be part of the normal program flow and / or will leave some external resources in an invalid state.
    CUSTOM_LOGGER_SEVERITY_CRIT,

    /// number of available severity options
    CUSTOM_LOGGER_SEVERITIES_NUMBER,
} custom_logger_severity_t;

/**
 * @brief Behavior when a logger is destroyed.
 *
 */
typedef enum custom_logger_on_destroy_t
{
    /// do nothing more than releasing the data from memory
    CUSTOM_LOGGER_ON_DESTROY_DO_NOTHING,
    /// in addition from releasing the data from memory, close the associated stream
    CUSTOM_LOGGER_ON_DESTROY_CLOSE_STREAM,
} custom_logger_on_destroy_t;

/**
 * @brief Opaque type to handle a statically-allocated custom logger's data.
 *
 */
typedef struct custom_logger_t custom_logger_t;

/**
 * @brief Creates a logger object on the static logger module memory and return a pointer to it.
 *
 * @param[in] target
 * @return custom_logger_t*
 */
custom_logger_t *
custom_logger_create(FILE *target, custom_logger_on_destroy_t on_destroy);

/**
 * @brief Release a logger object from the module memory.
 *
 * @param[inout] custom_logger_t
 */
void
custom_logger_destroy(custom_logger_t **custom_logger_t);

/**
 * @brief Log a message associated to a severity through a custom logger.
 *
 * @param[in] logger
 * @param[in] severity
 * @param[in] msg
 * @param[in] ...
 */
void
custom_logger_log(custom_logger_t *logger, custom_logger_severity_t severity, char *msg, ...);

#endif
