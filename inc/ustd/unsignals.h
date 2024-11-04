/**
 * @file unsignals.h
 * @author gabriel
 * @brief Lightweight signal framework to work with embedded systems.
 *
 * @see ustd_impl/unsignals_impl.h
 *
 * @version 0.1
 * @date 2024-11-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __UNSIGNALS_H__
#define __UNSIGNALS_H__

/**
 * @brief Signal return code.
 *
 */
enum SIGNAL_status {
    /// Everything went right (that we know of)
    SIGNAL_STATUS_OK,

    /// No space was left to subscribe to a signal.
    SIGNAL_STATUS_OUT_OF_MEM,
    /// Some NULL pointer was passed to a function, while it was expecting a valid object.
    SIGNAL_STATUS_INVALID_OBJECT,
    /// An operation over a subscription was attempted, but this subscription was not found.
    SIGNAL_STATUS_NOT_FOUND,
    /// Tried to add a subscription, but some other subscription already matches the function pointer.
    SIGNAL_STATUS_ALREADY_SUBSCRIBED,
};

/// a
#define SIGNAL(name_, signal_type_) \
        enum SIGNAL_status SIGNAL_##name_##_hook(void (*f)(signal_type_)); \
        enum SIGNAL_status SIGNAL_##name_##_unhook(void (*f)(signal_type_)); \
        enum SIGNAL_status SIGNAL_##name_##_send(signal_type_);

#endif
