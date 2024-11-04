
#include <stddef.h>
#include <stdint.h>

#include <unsignals.h>

static uint32_t signal_type_erased_find(const size_t n, uintptr_t functions[n], uintptr_t needle, size_t *out_pos);

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param n
 * @param functions
 * @param inserted
 * @return enum SIGNAL_status
 */
enum SIGNAL_status signal_type_erased_insert(const size_t n, uintptr_t functions[n], uintptr_t inserted)
{
    size_t pos = 0;

    if (!functions || !inserted) {
        return SIGNAL_STATUS_INVALID_OBJECT;
    }

    if (signal_type_erased_find(n, functions, inserted, NULL)) {
        return SIGNAL_STATUS_ALREADY_SUBSCRIBED;
    }

    while (pos < n) {
        if (!functions[pos]) {
            functions[pos] = inserted;
            break;
        }
        pos += 1;
    }

    if (pos == n) {
        return SIGNAL_STATUS_OUT_OF_MEM;
    }

    return SIGNAL_STATUS_OK;
}

/**
 * @brief
 *
 * @param n
 * @param functions
 * @param removed
 * @return enum SIGNAL_status
 */
enum SIGNAL_status signal_type_erased_remove(const size_t n, uintptr_t functions[n], uintptr_t removed)
{
    size_t pos = 0;

    if (!functions || !removed) {
        return SIGNAL_STATUS_INVALID_OBJECT;
    }

    if (signal_type_erased_find(n, functions, removed, &pos)) {
        functions[pos] = (uintptr_t) NULL;
    } else {
        return SIGNAL_STATUS_NOT_FOUND;
    }

    return SIGNAL_STATUS_OK;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param n
 * @param functions
 * @param needle
 * @param out_pos
 * @return uint32_t
 */
static uint32_t signal_type_erased_find(const size_t n, uintptr_t functions[n], uintptr_t needle, size_t *out_pos)
{
    size_t pos = 0;

    if (!functions || !needle) {
        return 0;
    }

    while ((pos < n) && (functions[pos] != needle)) {
        pos += 1;
    }

    if (out_pos) {
        *out_pos = pos;
    }

    return (pos < n);
}
