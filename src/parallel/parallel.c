
#include <ustd/parallel.h>
#include <ustd_impl/parallel_impl.h>

/**
 * @brief
 *
 * @param nb_workers
 * @param alloc
 * @return struct uthread_master*
 */
struct uthread_master *uthread_master_create(size_t nb_workers, struct allocator alloc)
{
    struct uthread_master *new_master = NULL;

    if (nb_workers == 0u) {
        return NULL;
    }

    new_master = alloc.malloc(alloc, sizeof(*new_master));
    if (new_master) {
        *new_master = (struct uthread_master) {
                .alloc      = alloc,
                .task_queue = range_create_dynamic(alloc, sizeof(*new_master->task_queue->data), 8u),
                .workers    = range_create_dynamic(alloc, sizeof(*new_master->workers->data), 8u),
        };
    }

    return new_master;
}

/**
 * @brief
 *
 * @param tm
 */
void uthread_master_destroy(struct uthread_master **tm)
{
    struct allocator used_alloc = { };

    if (!tm || !*tm) {
        return;
    }

    // TODO : destroy all tasks
    // TODO : destroy all workers

    used_alloc = (*tm)->alloc;

    range_destroy_dynamic(used_alloc, &RANGE_TO_ANY((*tm)->workers));
    range_destroy_dynamic(used_alloc, &RANGE_TO_ANY((*tm)->task_queue));

    used_alloc.free(used_alloc, *tm);
    *tm = NULL;
}

/**
 * @brief
 *
 * @param tm
 * @param routine
 * @param payout_size
 * @return
 */
uthread_handle uthread_start(struct uthread_master *tm, uthread_routine routine, void *args, size_t args_size, size_t payout_size)
{
    if (!routine || !tm || (!args && args_size > 0u)) {
        return UTHREAD_HANDLE_INVALID;
    }


}
