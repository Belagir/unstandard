
#ifndef __UNSTANDARD_PARALLEL_IMPL_H__
#define __UNSTANDARD_PARALLEL_IMPL_H__

#include <pthread.h>

#include "../ustd/range.h"

#include "../ustd/parallel.h"

enum thread_task_state {
    THREAD_TASK_STATE_INVALID,
    THREAD_TASK_STATE_UNSTARTED,
    THREAD_TASK_STATE_RUNNING,
    THREAD_TASK_STATE_FINISHED,
};

struct uthread_task {
    uthread_handle handle;
    uthread_routine call;
    enum thread_task_state state;

    void *args;
    size_t payout_size;
    void *payout;
};

struct uthread_worker {
    struct uthread_task current_task;

    pthread_t thread;
};

struct uthread_master {
    struct allocator alloc;
    RANGE(struct uthread_task) *task_queue;
    RANGE(struct uthread_worker) *workers;
};

struct uthread_task uthread_task_create(uthread_routine call, void *args, size_t args_size, size_t payout_size, struct allocator alloc);
struct uthread_task uthread_task_destroy(struct uthread_task *task, struct allocator alloc);

struct uthread_worker uthread_worker_create(struct allocator alloc);
void uthread_worker_destroy(struct uthread_worker *worker, struct allocator alloc);


#endif
