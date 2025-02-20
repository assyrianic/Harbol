#include "objpool.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT NO_NULL bool harbol_objpool_init(struct HarbolObjPool *const objpool, size_t const objsize, size_t const len) {
	if( len==0 || objsize==0 ) {
		return false;
	}
	
	size_t const aligned_objsize = harbol_align_size(objsize, sizeof aligned_objsize);
	objpool->mem = calloc(len, aligned_objsize);
	if( objpool->mem==nullptr ) {
		return false;
	}
	
	objpool->size = objpool->free_blocks = len;
	objpool->objsize = aligned_objsize;
	for( size_t i=0; i < objpool->free_blocks; i++ ) {
		size_t *const restrict index = ( size_t* )(&objpool->mem[i * objpool->objsize]);
		*index = i + 1;
	}
	objpool->next = ( uintptr_t )(objpool->mem);
	return true;
}

HARBOL_EXPORT struct HarbolObjPool harbol_objpool_make(size_t const objsize, size_t const len, bool *const res) {
	struct HarbolObjPool objpool = {0};
	*res = harbol_objpool_init(&objpool, objsize, len);
	return objpool;
}

HARBOL_EXPORT NO_NULL bool harbol_objpool_init_from_buffer(struct HarbolObjPool *const restrict objpool, void *const restrict buf, size_t const objsize, size_t const len) {
	/// If the object index isn't large enough to align to a size_t, then we can't use it.
	if( objsize < sizeof objsize || (objsize * len) < (harbol_align_size(objsize, sizeof objsize) * len) ) {
		return false;
	}
	
	objpool->objsize = harbol_align_size(objsize, sizeof objsize);
	objpool->size = objpool->free_blocks = len;
	objpool->mem = buf;
	for( size_t i=0; i < objpool->free_blocks; i++ ) {
		size_t *const restrict index = ( size_t* )(&objpool->mem[i * objpool->objsize]);
		*index = i + 1;
	}
	objpool->next = ( uintptr_t )(objpool->mem);
	return true;
}

HARBOL_EXPORT struct HarbolObjPool harbol_objpool_from_buffer(void *const restrict buf, size_t const objsize, size_t const len, bool *const restrict res) {
	struct HarbolObjPool objpool = {0};
	*res = harbol_objpool_init_from_buffer(&objpool, buf, objsize, len);
	return objpool;
}

HARBOL_EXPORT void harbol_objpool_clear(struct HarbolObjPool *const objpool) {
	if( objpool->mem==nullptr ) {
		return;
	}
	free(objpool->mem);
	*objpool = ( struct HarbolObjPool ){0};
}

HARBOL_EXPORT void *harbol_objpool_alloc(struct HarbolObjPool *const objpool) {
	if( objpool->free_blocks==0 ) {
		return nullptr;
	}
	/// for first allocation, head points to the very first index.
	/// next = &pool[0];
	/// ret = next == ret = &pool[0];
	size_t *const index = ( size_t* )(objpool->next);
	objpool->free_blocks--;
	
	/// after allocating, we set head to the address of the index that *next holds.
	/// next = &pool[*next * pool.objsize];
	objpool->next = ( objpool->free_blocks != 0 )? ( uintptr_t )(&objpool->mem[*index * objpool->objsize]) : intnullptr;
	return memset(index, 0, objpool->objsize);
}

HARBOL_EXPORT void harbol_objpool_free(struct HarbolObjPool *const restrict objpool, void *const ptrref) {
	void **p_ref = ptrref;
	if( *p_ref==nullptr
			|| ( uint8_t const* )(*p_ref) < objpool->mem
			|| ( uint8_t const* )(*p_ref) > &objpool->mem[objpool->size * objpool->objsize]
	) {
		return;
	}
	
	/**
	 * when we free our pointer, we recycle the pointer space to store the previous index
	 * and then we push it as our new head.
	 * *p = index of next in relation to the buffer;
	 * next = p;
	 */
	{
		size_t *const restrict index = *p_ref;
		*index = ( objpool->next != intnullptr )? (objpool->next - ( uintptr_t )(objpool->mem)) / objpool->objsize : objpool->size;
	}
	objpool->next = ( uintptr_t )(*p_ref);
	++objpool->free_blocks;
	*p_ref = nullptr;
}

HARBOL_EXPORT void harbol_objpool_cleanup(struct HarbolObjPool *const restrict objpool, void *const restrict ptrref) {
	void **ref = ptrref;
	if( *ref==nullptr ) {
		return;
	}
	harbol_objpool_free(objpool, *ref);
	*ref = nullptr;
}