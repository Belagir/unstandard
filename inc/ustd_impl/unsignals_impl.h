
#ifndef __UNSIGNALS_IMPL_H__
#define __UNSIGNALS_IMPL_H__

#include <stddef.h>
#include <stdint.h>

#include <unsignals.h>

#define SIGNAL_STATIC_DATA(name_, max_capacity_, signal_type_) \
        static struct { union { void (*f[max_capacity_])(signal_type_); uintptr_t raw[max_capacity_]; } subscribers; } static_SIGNAL_##name_##_handler = { 0u }

#define SIGNAL_HOOK_DEFINITION(name_, max_capacity_, signal_type_) \
        enum SIGNAL_status SIGNAL_##name_##_hook(void (*f)(signal_type_)) { \
            return signal_type_erased_insert(max_capacity_, static_SIGNAL_##name_##_handler.subscribers.raw, (uintptr_t) f); \
        }

#define SIGNAL_UNHOOK_DEFINITION(name_, max_capacity_, signal_type_) \
        enum SIGNAL_status SIGNAL_##name_##_unhook(void (*f)(signal_type_)) { \
            return signal_type_erased_remove(max_capacity_, static_SIGNAL_##name_##_handler.subscribers.raw, (uintptr_t) f); \
        }

#define SIGNAL_SEND_DEFINITION(name_, max_capacity_, signal_type_) \
        enum SIGNAL_status SIGNAL_##name_##_send(signal_type_ message) { \
            for (size_t i = 0 ; i < max_capacity_ ; i++) { \
                if (static_SIGNAL_##name_##_handler.subscribers.f[i]) static_SIGNAL_##name_##_handler.subscribers.f[i](message); \
            } \
            return SIGNAL_STATUS_OK; \
        }

#define SIGNAL_STATIC_MACHINERY(name_, max_capacity_, signal_type_) \
        SIGNAL_STATIC_DATA(name_, max_capacity_, signal_type_); \
        \
        SIGNAL_HOOK_DEFINITION(name_, max_capacity_, signal_type_) \
        SIGNAL_UNHOOK_DEFINITION(name_, max_capacity_, signal_type_) \
        SIGNAL_SEND_DEFINITION(name_, max_capacity_, signal_type_)

enum SIGNAL_status signal_type_erased_insert(const size_t n, uintptr_t functions[n], uintptr_t inserted);
enum SIGNAL_status signal_type_erased_remove(const size_t n, uintptr_t functions[n], uintptr_t removed);

#endif
