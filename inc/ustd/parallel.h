
#ifndef __UNSTANDARD_PARALLEL_H__
#define __UNSTANDARD_PARALLEL_H__

#include "allocation.h"
#include "common.h"

typedef u32 uthread_handle;

constexpr uthread_handle UTHREAD_HANDLE_INVALID = { 0u };

typedef void (*uthread_routine)(void *args, void *payout);

struct uthread_master;

struct uthread_master *uthread_master_create(size_t nb_workers, struct allocator alloc);
void uthread_master_destroy(struct uthread_master **tm);

uthread_handle uthread_start(struct uthread_master *tm, uthread_routine routine, void *args, size_t args_size, size_t payout_size);
void uthread_wait(struct uthread_master *tm, uthread_handle task, void *payout);

#endif
