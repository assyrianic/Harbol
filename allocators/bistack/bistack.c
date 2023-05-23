#include "bistack.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT bool harbol_bistack_init(struct HarbolBiStack *const bistk, size_t const len) {
	if( len==0 ) {
		return false;
	}
	bistk->mem = calloc(len, sizeof *bistk->mem);
	if( bistk->mem==NULL ) {
		return false;
	}
	bistk->size = bistk->back = len;
	return true;
}

HARBOL_EXPORT struct HarbolBiStack harbol_bistack_make(size_t const len, bool *const res) {
	struct HarbolBiStack bistk = {0};
	*res = harbol_bistack_init(&bistk, len);
	return bistk;
}

HARBOL_EXPORT struct HarbolBiStack harbol_bistack_make_from_buffer(void *const buf, size_t const len) {
	struct HarbolBiStack bistk = {0};
	bistk.size  = len;
	bistk.mem   = buf;
	bistk.back  = len;
	return bistk;
}

HARBOL_EXPORT void harbol_bistack_clear(struct HarbolBiStack *const bistk) {
	if( bistk->mem==NULL ) {
		return;
	}
	free(bistk->mem); bistk->mem = NULL;
	bistk->front = bistk->back = bistk->size = 0;
}

HARBOL_EXPORT void *harbol_bistack_alloc_front(struct HarbolBiStack *const bistk, size_t const size) {
	if( bistk->mem==NULL ) {
		return NULL;
	}
	
	size_t const aligned_size = harbol_align_size(size, sizeof aligned_size);
	/// front end arena is too high!
	if( bistk->front + aligned_size >= bistk->back ) {
		return NULL;
	}
	size_t const f = bistk->front;
	bistk->front += aligned_size;
	return bistk->mem + f;
}

HARBOL_EXPORT void *harbol_bistack_alloc_back(struct HarbolBiStack *const restrict bistk, size_t const size) {
	if( bistk->mem==NULL ) {
		return NULL;
	}
	
	size_t const aligned_size = harbol_align_size(size, sizeof aligned_size);
	/// back end arena is too low
	if( bistk->back - aligned_size <= bistk->front ) {
		return NULL;
	}
	bistk->back -= aligned_size;
	return bistk->mem + bistk->back;
}

HARBOL_EXPORT void harbol_bistack_reset_front(struct HarbolBiStack *const bistk) {
	if( bistk->mem==NULL ) {
		return;
	}
	bistk->front = 0;
}

HARBOL_EXPORT void harbol_bistack_reset_back(struct HarbolBiStack *const bistk) {
	if( bistk->mem==NULL ) {
		return;
	}
	bistk->back = bistk->size;
}

HARBOL_EXPORT void harbol_bistack_reset_all(struct HarbolBiStack *const bistk) {
	if( bistk->mem==NULL ) {
		return;
	}
	bistk->front = 0;
	bistk->back  = bistk->size;
}

HARBOL_EXPORT size_t harbol_bistack_get_margins(struct HarbolBiStack const bistk) {
	return bistk.back - bistk.front;
}


HARBOL_EXPORT bool harbol_bistack_resize(struct HarbolBiStack *const restrict bistk, size_t const new_size) {
	uint8_t *const new_buf = harbol_recalloc(( void* )(bistk->mem), new_size, sizeof *new_buf, bistk->size);
	if( new_buf==NULL ) {
		return false;
	}
	bistk->mem   = new_buf;
	bistk->size  = bistk->back = new_size;
	bistk->front = 0;
	return true;
}
