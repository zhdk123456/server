#ifndef CACHETABLE_H
#define CACHETABLE_H

#include <fcntl.h>

/* Implement the cache table. */

typedef long long CACHEKEY;
typedef struct cachetable *CACHETABLE;
typedef struct cachefile *CACHEFILE;

/* Maintain a cache mapping from cachekeys to values (void*)
 * Some of the keys can be pinned.  Don't pin too many or for too long.
 * If the cachetable is too full, it will call the flush_callback() function with the key, the value, and the otherargs
   and then remove the key-value pair from the cache.
 * The callback won't be any of the currently pinned keys.
 * Also when flushing an object, the cachetable drops all references to it,
 * so you may need to free() it.
 * Note: The cachetable should use a common pool of memory, flushing things across cachetables.
 *  (The first implementation doesn't)
 * If you pin something twice, you must unpin it twice.
 * table_size is the initial size of the cache table hash table
 * size limit is the upper bound of the sum of size of the entries in the cache table
 */
int create_cachetable_size(CACHETABLE */*result*/, int table_size, long size_limit);

int cachetable_openf (CACHEFILE *,CACHETABLE, const char */*fname*/, int flags, mode_t mode);

typedef void (*cachetable_flush_func_t)(CACHEFILE, CACHEKEY key, void*value, long size, int write_me, int keep_me);

/* If we are asked to fetch something, get it by calling this back. */
typedef int (*cachetable_fetch_func_t)(CACHEFILE, CACHEKEY key, void **value, long *sizep, void *extraargs); 

/* Error if already present.  On success, pin the value. */
int cachetable_put_size(CACHEFILE cf, CACHEKEY key, void* value, long size,
                    cachetable_flush_func_t flush_callback, cachetable_fetch_func_t fetch_callback, void *extraargs);

int cachetable_get_and_pin_size(CACHEFILE, CACHEKEY, void**/*value*/, long *sizep,
                            cachetable_flush_func_t flush_callback, cachetable_fetch_func_t fetch_callback, void *extraargs);

/* If the the item is already in memory, then return 0 and store it in the void**.
 * If the item is not in memory, then return nonzero. */
int cachetable_maybe_get_and_pin (CACHEFILE, CACHEKEY, void**);
int cachetable_unpin_size (CACHEFILE, CACHEKEY, int dirty, long size); /* Note whether it is dirty when we unpin it. */
int cachetable_remove (CACHEFILE, CACHEKEY, int /*write_me*/); /* Removing something already present is OK. */
int cachetable_assert_all_unpinned (CACHETABLE);
int cachefile_count_pinned (CACHEFILE, int /*printthem*/ );

//int cachetable_fsync_all (CACHETABLE); /* Flush everything to disk, but keep it in cache. */
int cachetable_close (CACHETABLE*); /* Flushes everything to disk, and destroys the cachetable. */

int cachefile_close (CACHEFILE*);
//int cachefile_flush (CACHEFILE); /* Flush everything related to the VOID* to disk and free all memory.  Don't destroy the cachetable. */

// Return on success (different from pread and pwrite)
//int cachefile_pwrite (CACHEFILE, const void *buf, size_t count, off_t offset);
//int cachefile_pread  (CACHEFILE, void *buf, size_t count, off_t offset);

int cachefile_fd (CACHEFILE);

// Useful for debugging 
void cachetable_print_state (CACHETABLE ct);
void cachetable_get_state(CACHETABLE ct, int *num_entries_ptr, int *hash_size_ptr, long *size_current_ptr, long *size_limit_ptr);
int cachetable_get_key_state(CACHETABLE ct, CACHEKEY key, void **value_ptr,
                         int *dirty_ptr, long long *pin_ptr, long *size_ptr);

// Compat, will be removed
static inline int create_cachetable (CACHETABLE *result, int table_size) {
    return create_cachetable_size(result, table_size, table_size);
}

static inline int cachetable_put(CACHEFILE cf, CACHEKEY key, void* value,
                                 cachetable_flush_func_t flush_callback, cachetable_fetch_func_t fetch_callback, void *extraargs) {
    return cachetable_put_size(cf, key, value, 1, flush_callback, fetch_callback, extraargs);
}

static inline int cachetable_get_and_pin (CACHEFILE cf, CACHEKEY key, void **valuep, 
                                          cachetable_flush_func_t flush_callback, cachetable_fetch_func_t fetch_callback, 
                                          void *extraargs) {
    long size;
    return cachetable_get_and_pin_size(cf, key, valuep, &size, flush_callback, fetch_callback, extraargs);
}

static inline int cachetable_unpin(CACHEFILE cf, CACHEKEY key, int dirty) {
    return cachetable_unpin_size(cf, key, dirty, 1);
}

#endif
