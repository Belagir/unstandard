
#include "ouroboros.h"

struct orbr_block_t;
/**
 * @brief This structure contains the data needed to identify a block of allocated memory
 * Sometimes referenced as the "header" of the block.
 */
typedef struct orbr_block_t {
    /// address of the next block, this address can be *before* this block's
    struct orbr_block_t *next;
    /// size, in number of bytes, of the block
    size_t size;
} orbr_block_t;

#define BLOCK_HEADER_SIZE sizeof(orbr_block_t)  ///< size in number of bytes of a block header
#define BLOCK_TOTAL_SIZE(_ptr_block_header) ((BLOCK_HEADER_SIZE)*(_ptr_block_header->size != 0) + (_ptr_block_header->size)) ///< total number of bytes taken by the block
#define BLOCK_OF(_ptr_memory) ((orbr_block_t *) ((size_t) _ptr_memory - BLOCK_HEADER_SIZE)) ///< inferred block of a pointer. Might not exist, no way to know.

/**
 * @brief Data describing an Ouroboros allocator.
 * This is constituted of cyclically chained blocks of allocated memory. There is at least one block.
 */
typedef struct ouroboros_t {
    /// pointer to the last allocated block
    orbr_block_t *index;
    /// pointer to the leased memory that the allocator will exploit
    void *raw_mem;
    /// size, in number of bytes, of the memory
    size_t size_memory;
    /// number of bytes that have been allocated int total.
    size_t nb_bytes_allocated;
} ouroboros_t;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the free size from the allocator's static data's current index to the next block
 * or the end of the buffer.
 *
 * @return size_t number of bytes free after the currently indexed block.
 */
static size_t alloc_free_size_at_index(ouroboros_t *alloc);

/**
 * @brief Gets rid of all the zero-sized blocks following the currently indexed block.
 */
static void alloc_cleanup_at_index(ouroboros_t *alloc);

/**
 * @brief Initialize an allocator to an empty state.
 *
 * @param alloc target allocator
 * @param size size of the memory managed by the allocator
 */
static void alloc_initialize(ouroboros_t *alloc, size_t size);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
void *orbr_access_bounded_raw(void *target_array, size_t position) {
    return (void *) ((((size_t) target_array + position)) * (position < (BLOCK_OF(target_array)->size)));
}


// -------------------------------------------------------------------------------------------------
void * orbr_alloc(ouroboros_t *alloc, const size_t wanted_size) {
    // calculated free size after an arbitrary block
    size_t free_size = 0u;
    // eventual address of the new block
    orbr_block_t *new_block_address = 0x0;
    orbr_block_t new_block = { 0 };
    // boolean value for looping status around the buffer
    int is_looping = 0;
    // boolean value for the available space during search
    int has_space = 0;

    // to prevent infinite loop
    const orbr_block_t *start_block = alloc->index;
    // actual size needed from the allocator
    const size_t real_wanted_size = wanted_size + BLOCK_HEADER_SIZE;

    // even without knowing the state of the fragmentation, we can already use the total remaining
    // number of bytes as a first screening
    if (real_wanted_size > (alloc->size_memory - alloc->nb_bytes_allocated)) {
        return 0x0;
    }

    // walk through all blocks once to search for a big enough space
    do {
        alloc_cleanup_at_index(alloc);
        free_size = alloc_free_size_at_index(alloc);
        has_space = (free_size >= real_wanted_size);

        if (!has_space) {
            alloc->index = alloc->index->next;
            // checking with an interval in case the starting block was deleted
            is_looping = ((alloc->index <= start_block) && (alloc->index->next >= start_block));
        }
    } while ((!has_space) && (!is_looping));

    // no space big enough found, but there is enough bytes in total : fragmentation time !
    if (free_size < real_wanted_size) {
        return 0x0;
    }

    // block creation
    new_block = (orbr_block_t) {
            .next = alloc->index->next,
            .size = wanted_size,
    };

    // if the index is on a deleted block, we have to replace it rather than creating a new one at the end
    if ((alloc->index->size == 0)) {
        new_block_address = alloc->index;
    } else {
        new_block_address = (orbr_block_t *) (((byte *) alloc->index) + BLOCK_TOTAL_SIZE(alloc->index));
    }

    *new_block_address = new_block;

    alloc->index->next = new_block_address;
    alloc->index = new_block_address;

    // keeping count
    alloc->nb_bytes_allocated += BLOCK_TOTAL_SIZE(new_block_address);

    return (void *) ((byte *) (new_block_address) + BLOCK_HEADER_SIZE);
}

// -------------------------------------------------------------------------------------------------
void orbr_clear(ouroboros_t *alloc) {
    alloc_initialize(alloc, alloc->size_memory + sizeof(*alloc));
}

// -------------------------------------------------------------------------------------------------
ouroboros_t *orbr_create(void *mem, const size_t size) {
    ouroboros_t *allocator = (ouroboros_t *) mem;
    alloc_initialize(allocator, size);

    return allocator;
}

// -------------------------------------------------------------------------------------------------
void orbr_free(ouroboros_t *alloc, void *object) {
    // checking if the address is actually managed by the allocator
    if (((byte *) object < ((byte *) alloc->raw_mem + BLOCK_HEADER_SIZE)) || ((byte *) object >= ((byte *) alloc->raw_mem + alloc->size_memory))) {
        return;
    }

    // fetching the block
    // let's hope there is actually a block there
    orbr_block_t *obj_block_address = BLOCK_OF(object);

    // keeping count, again
    alloc->nb_bytes_allocated -= BLOCK_TOTAL_SIZE(obj_block_address);

    // setting the block as deleted, invalid
    obj_block_address->size = 0u;
}

// -------------------------------------------------------------------------------------------------
void *orbr_realloc(ouroboros_t *alloc, void *object, size_t new_size) {
    orbr_block_t *original_block_address = BLOCK_OF(object);
    size_t original_size = original_block_address->size;
    void *new_object_address;

    if (new_size <= original_size) {
        original_block_address->size = new_size;
        return object;
    }

    // counting the block as invalid
    original_block_address->size = 0u;
    new_object_address = orbr_alloc(alloc, new_size);

    if (new_object_address) {
        // allocation successfull, we need to copy contents
        bytewise_copy(new_object_address, object, original_size);
    } else {
        // allocation didn't pass through, paddling back, returning null
        new_object_address = 0x0;
        original_block_address->size = original_size;
    }

    return new_object_address;
}

// -------------------------------------------------------------------------------------------------
size_t orbr_space_used(ouroboros_t *alloc) {
    return alloc->nb_bytes_allocated + sizeof(*alloc);
}

// -------------------------------------------------------------------------------------------------
static size_t alloc_free_size_at_index(ouroboros_t *alloc) {
    size_t free_chunk_size = 0u;
    void *limit = (void *) alloc->index->next;

    // the currently indexed block is loops back to the beginning, available memory is the buffer's end
    if (alloc->index->next <= alloc->index) {
        limit = ((byte *) alloc->raw_mem) + alloc->size_memory;
    }

    free_chunk_size = (size_t) ((byte *) limit - ((byte *) (alloc->index) + BLOCK_TOTAL_SIZE(alloc->index)));

    return free_chunk_size;
}

// -------------------------------------------------------------------------------------------------
static void alloc_cleanup_at_index(ouroboros_t *alloc) {
    // every block not looping back that has no size is invalid and must be ignored.
    while ((alloc->index < alloc->index->next) && (alloc->index->next->size == 0)) {
        alloc->index->next = alloc->index->next->next;
    }
}

// -------------------------------------------------------------------------------------------------
static void alloc_initialize(ouroboros_t *alloc, size_t size) {
    *alloc = (ouroboros_t) {
            .index              = (orbr_block_t *) ((byte*) alloc) + sizeof(*alloc),
            .nb_bytes_allocated = 0u,
            .size_memory        = size - sizeof(*alloc),
            .raw_mem            = (orbr_block_t *) ((byte*) alloc) + sizeof(*alloc),
    };

    // first block at the start of the allocated memory as a starting point
    *(alloc->index) = (orbr_block_t) {
            .next = alloc->index,
            .size = 0u
    };
}

