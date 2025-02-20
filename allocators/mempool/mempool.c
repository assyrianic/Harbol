#include "mempool.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


static void _setup_sizes(struct HarbolMemPool *const mempool, size_t const bytes, size_t const block_size) {
	mempool->mem_len = bytes;
	size_t const size_width = harbol_size_bits(sizeof size_width);
	mempool->blk_size = block_size;
	mempool->bm_len = harbol_align_size(bytes / block_size, size_width) / size_width;
}

HARBOL_EXPORT bool harbol_mempool_init(struct HarbolMemPool *const mempool, size_t const bytes, size_t const block_size) {
	size_t const aligned_bytes = harbol_align_size(bytes, block_size);
	_setup_sizes(mempool, aligned_bytes, block_size);
	mempool->mem = calloc(aligned_bytes, sizeof *mempool->mem);
	mempool->bitmap = calloc(mempool->bm_len, sizeof *mempool->bitmap);
	if( mempool->mem==nullptr || mempool->bitmap==nullptr ) {
		harbol_multi_cleanup(0, &mempool->mem, &mempool->bitmap, nullptr);
		return false;
	}
	return true;
}

HARBOL_EXPORT bool harbol_mempool_init_buffer(struct HarbolMemPool *const restrict mempool, void *const buffer, size_t const bytes, size_t const block_size) {
	if( !is_size_aligned(bytes, block_size) ) {
		return false;
	}
	_setup_sizes(mempool, bytes, block_size);
	mempool->mem = buffer;
	mempool->bitmap = calloc(mempool->bm_len, sizeof *mempool->bitmap);
	if( mempool->bitmap==nullptr ) {
		return false;
	}
	return true;
}

HARBOL_EXPORT struct HarbolMemPool harbol_mempool_make(size_t const bytes, size_t const block_size, bool *const restrict res) {
	struct HarbolMemPool mempool = {0};
	*res = harbol_mempool_init(&mempool, bytes, block_size);
	return mempool;
}

HARBOL_EXPORT struct HarbolMemPool harbol_mempool_make_buffer(void *const buffer, size_t const bytes, size_t const block_size, bool *const restrict res) {
	struct HarbolMemPool mempool = {0};
	*res = harbol_mempool_init_buffer(&mempool, buffer, bytes, block_size);
	return mempool;
}

HARBOL_EXPORT void harbol_mempool_clear(struct HarbolMemPool *const mempool, bool const mem_is_buffer) {
	if( !mem_is_buffer ) {
		harbol_cleanup(&mempool->mem);
	}
	harbol_cleanup(&mempool->bitmap);
	*mempool = ( struct HarbolMemPool ){0};
}

HARBOL_EXPORT void *harbol_mempool_alloc(struct HarbolMemPool *const mempool, size_t const bytes) {
	if( bytes==0 || bytes > mempool->mem_len ) {
		return nullptr;
	}
	
	size_t const aligned_bytes = harbol_align_size(bytes, mempool->blk_size); /// i.e. 128
	size_t const req_num_blks = aligned_bytes / mempool->blk_size; /// 128 / 64 == 2
	size_t const mask = harbol_bit_mask(req_num_blks); /// (1 << 2) - 1 = 4 - 1 = 3
	size_t const total_blks = mempool->mem_len / mempool->blk_size;
	HARBOL_DBG_PRINT("aligned_bytes: %zu | requested bytes: %zu | blksize: %zu | req_num_blks: %zu | mask: %#zx", aligned_bytes, bytes, mempool->blk_size, req_num_blks, mask);
	for( size_t i=0; i < mempool->bm_len; i++ ) {
		for( size_t n=0; n <= (harbol_size_bits(sizeof n) - req_num_blks); n++ ) {
			size_t const shifted_mask = mask << n;
			if( (mempool->bitmap[i] & shifted_mask)==0 ) {
				mempool->bitmap[i] |= shifted_mask;
				size_t const blk_idx = (i * harbol_size_bits(sizeof n)) + n;
				if( blk_idx + req_num_blks > total_blks ) {
					continue;
				}
				uint8_t *const allocation = &mempool->mem[blk_idx * mempool->blk_size];
				HARBOL_DBG_PRINT("mempool->bitmap[%zu]: %zu | mask(%#zx) << n(%zu) = %zu | blk_idx: %zu | offset: %zu", i, mempool->bitmap[i], mask, n, shifted_mask, blk_idx, blk_idx * mempool->blk_size);
				return memset(allocation, 0, aligned_bytes);
			}
		}
	}
	return nullptr;
}

HARBOL_EXPORT void *harbol_mempool_realloc(struct HarbolMemPool *const restrict mempool, void *const old_ptr_ref, size_t const old_size, size_t const new_size) {
	void **p_ref = old_ptr_ref;
	if( new_size > mempool->mem_len ) {
		return nullptr;
	} else if( new_size==old_size ) {
		return *p_ref;
	}
	
	void *const new_ptr = harbol_mempool_alloc(mempool, new_size);
	harbol_mempool_free(mempool, p_ref, old_size);
	return new_ptr;
}

HARBOL_EXPORT bool harbol_mempool_free(struct HarbolMemPool *const restrict mempool, void *const ptr_ref, size_t const bytes) {
	void **p_ref = ptr_ref;
	if( p_ref==nullptr || bytes==0 || *p_ref==nullptr ) {
		return false;
	}
	
	size_t const offs = get_byte_diff(*p_ref, mempool->mem);
	HARBOL_DBG_PRINT("offs: %zu", offs);
	if( !is_size_aligned(offs, mempool->blk_size) || offs >= mempool->mem_len ) {
		return false;
	}
	
	size_t const start_blk = offs / mempool->blk_size;
	size_t const req_num_blks = harbol_align_size(bytes, mempool->blk_size) / mempool->blk_size;
	size_t const size_width = harbol_size_bits(sizeof size_width);
	HARBOL_DBG_PRINT("start_blk: %zu | req_num_blks: %zu | size_width: %zu", start_blk, req_num_blks, size_width);
	for( size_t i=0; i < req_num_blks; i++ ) {
		size_t const target_blk = start_blk + i;
		size_t const bm_idx = target_blk / size_width;
		size_t const bm_bit = target_blk % size_width;
		size_t const bm_bit_idx = harbol_bit_index(bm_bit);
		HARBOL_DBG_PRINT("target_blk: %zu | bm_bit: %zu | bm_bit_idx: %zu | ~bm_bit_idx: %#zx | mempool->bitmap[bm_idx(%zu)]: %#zx", target_blk, bm_bit, bm_bit_idx, ~bm_bit_idx, bm_idx, mempool->bitmap[bm_idx]);
		mempool->bitmap[bm_idx] &= ~bm_bit_idx;
	}
	*p_ref = nullptr;
	return true;
}

HARBOL_EXPORT size_t harbol_mempool_remaining(struct HarbolMemPool const *const mempool, bool const mem_left) {
	size_t total = 0;
	for( size_t i=0; i < mempool->bm_len; i++ ) {
		size_t const bits = mem_left? ~mempool->bitmap[i] : mempool->bitmap[i];
		total += harbol_popcount(bits);
	}
	return total * mempool->blk_size;
}

HARBOL_EXPORT void harbol_mempool_reset(struct HarbolMemPool *const mempool) {
	for( size_t i=0; i < mempool->bm_len; i++ ) {
		mempool->bitmap[i] = 0UL;
	}
}