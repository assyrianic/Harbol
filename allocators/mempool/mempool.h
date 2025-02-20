#ifndef HARBOL_MEMPOOL_INCLUDED
#	define HARBOL_MEMPOOL_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../../harbol_common_defines.h"
#include "../../harbol_common_includes.h"

/**
 * Bitmap Allocator.
 */
struct HarbolMemPool {
	uint8_t *mem;
	size_t  *bitmap;
	size_t   mem_len, bm_len, blk_size;
};

HARBOL_EXPORT NO_NULL bool harbol_mempool_init(struct HarbolMemPool *mempool, size_t bytes, size_t block_size);
HARBOL_EXPORT NO_NULL bool harbol_mempool_init_buffer(struct HarbolMemPool *mempool, void *buffer, size_t bytes, size_t block_size);
HARBOL_EXPORT NO_NULL struct HarbolMemPool harbol_mempool_make(size_t bytes, size_t block_size, bool *res);
HARBOL_EXPORT NO_NULL struct HarbolMemPool harbol_mempool_make_buffer(void *buffer, size_t bytes, size_t block_size, bool *res);
HARBOL_EXPORT NO_NULL void harbol_mempool_clear(struct HarbolMemPool *mempool, bool mem_is_buffer);

HARBOL_EXPORT NO_NULL void *harbol_mempool_alloc(struct HarbolMemPool *mempool, size_t bytes);
HARBOL_EXPORT NEVER_NULL(1) void *harbol_mempool_realloc(struct HarbolMemPool *mempool, void *old_ptr_ref, size_t old_size, size_t new_size);
HARBOL_EXPORT NO_NULL bool harbol_mempool_free(struct HarbolMemPool *mempool, void *ptr_ref, size_t bytes);

HARBOL_EXPORT NO_NULL size_t harbol_mempool_remaining(struct HarbolMemPool const *mempool, bool mem_left);
HARBOL_EXPORT NO_NULL void harbol_mempool_reset(struct HarbolMemPool *mempool);
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MEMPOOL_INCLUDED */