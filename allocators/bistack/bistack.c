#include "bistack.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT bool harbol_bistack_init(struct HarbolBiStack *const bistk, size_t const len) {
	if( len==0 ) {
		return false;
	}
	
	bistk->mem = calloc(len, sizeof *bistk->mem);
	if( bistk->mem==nullptr ) {
		return false;
	}
	bistk->size = bistk->offsets[0] = len;
	return true;
}

HARBOL_EXPORT struct HarbolBiStack harbol_bistack_make(size_t const len, bool *const res) {
	struct HarbolBiStack bistk = {0};
	*res = harbol_bistack_init(&bistk, len);
	return bistk;
}

HARBOL_EXPORT struct HarbolBiStack harbol_bistack_make_from_buffer(void *const buf, size_t const len) {
	struct HarbolBiStack bistk = {0};
	bistk.mem  = buf;
	bistk.size = bistk.offsets[0] = len;
	return bistk;
}

HARBOL_EXPORT void harbol_bistack_clear(struct HarbolBiStack *const bistk) {
	harbol_cleanup(&bistk->mem);
	bistk->offsets[1] = bistk->offsets[0] = bistk->size = 0;
}

static void *_harbol_bistack_gen_alloc(struct HarbolBiStack *const restrict bistk, size_t size, bool const is_front) {
	/**
	 * When allocating from the front, we need the offset to increase
	 * so we save the front, increase front, and use the offset we saved.
	 * 
	 * When allocating from the back, we decrease, then use that decreased offset.
	 */
	size = harbol_align_size(size, sizeof size);
	size_t const offs = bistk->offsets[is_front];
	size_t const opp = bistk->offsets[!is_front];
	size_t const total_offs = is_front? (offs + size) : (offs - size);
	if( (is_front && total_offs >= opp) || (!is_front && total_offs <= opp) ) {
		return nullptr;
	}
	bistk->offsets[is_front] = total_offs;
	return &bistk->mem[is_front? offs : total_offs];
}

HARBOL_EXPORT void *harbol_bistack_alloc_front(struct HarbolBiStack *const bistk, size_t const size) {
	if( bistk->mem==nullptr ) {
		return nullptr;
	}
	return _harbol_bistack_gen_alloc(bistk, size, true);
}

HARBOL_EXPORT void *harbol_bistack_alloc_back(struct HarbolBiStack *const restrict bistk, size_t const size) {
	if( bistk->mem==nullptr ) {
		return nullptr;
	}
	return _harbol_bistack_gen_alloc(bistk, size, false);
}

HARBOL_EXPORT void harbol_bistack_reset_front(struct HarbolBiStack *const bistk) {
	bistk->offsets[1] = 0;
}

HARBOL_EXPORT void harbol_bistack_reset_back(struct HarbolBiStack *const bistk) {
	bistk->offsets[0] = bistk->size;
}

HARBOL_EXPORT void harbol_bistack_reset_all(struct HarbolBiStack *const bistk) {
	bistk->offsets[1] = 0;
	bistk->offsets[0] = bistk->size;
}

HARBOL_EXPORT size_t harbol_bistack_get_margins(struct HarbolBiStack const *const bistk) {
	return bistk->offsets[0] - bistk->offsets[1];
}

HARBOL_EXPORT bool harbol_bistack_resize(struct HarbolBiStack *const restrict bistk, size_t const new_size) {
	uint8_t *const new_buf = harbol_recalloc(bistk->mem, new_size, sizeof *new_buf, bistk->size);
	if( new_buf==nullptr ) {
		return false;
	}
	bistk->mem        = new_buf;
	bistk->size       = bistk->offsets[0] = new_size;
	bistk->offsets[1] = 0;
	return true;
}