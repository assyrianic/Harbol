#include "deque.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

HARBOL_EXPORT struct HarbolDeque *harbol_deque_new(size_t const init_size) {
	struct HarbolDeque *deque = calloc(1, sizeof *deque);
	if( deque==NULL || !harbol_deque_init(deque, init_size) ) {
		free(deque);
		return NULL;
	}
	return deque;
}

HARBOL_EXPORT bool harbol_deque_init(struct HarbolDeque *const deque, size_t const init_size) {
	if( !harbol_multi_recalloc(init_size, deque->cap, 3,
						&deque->nexts, sizeof *deque->nexts,
						&deque->prevs, sizeof *deque->prevs,
						&deque->datum, sizeof *deque->datum) ) {
		return false;
	}
	
	
	deque->cap = init_size;
	for( size_t i=0; i < init_size; i++ ) {
		deque->prevs[i] = i - 1;
		deque->nexts[i] = i + 1;
	}
	deque->head = deque->tail = deque->nexts[deque->cap - 1] = SIZE_MAX;
	deque->freed = deque->cap - 1;
	return true;
}

HARBOL_EXPORT struct HarbolDeque harbol_deque_make(size_t const init_size, bool *const res) {
	struct HarbolDeque d = {0};
	*res = harbol_deque_init(&d, init_size);
	return d;
}

HARBOL_EXPORT void harbol_deque_reset(struct HarbolDeque *const deque) {
	for( size_t i=0; i < deque->cap; i++ ) {
		free(deque->datum[i]); deque->datum[i] = NULL;
		deque->prevs[i] = i - 1;
		deque->nexts[i] = i + 1;
	}
	deque->head = deque->tail = deque->nexts[deque->cap - 1] = SIZE_MAX;
	deque->freed = deque->cap - 1;
}

HARBOL_EXPORT void harbol_deque_clear(struct HarbolDeque *const deque) {
	for( size_t i=0; i < deque->cap; i++ ) {
		free(deque->datum[i]); deque->datum[i] = NULL;
	}
	harbol_multi_cleanup(3, &deque->nexts, &deque->prevs, &deque->datum);
	*deque = ( struct HarbolDeque ){0};
}
HARBOL_EXPORT void harbol_deque_free(struct HarbolDeque **const dequeref) {
	if( *dequeref==NULL ) {
		return;
	}
	harbol_deque_clear(*dequeref);
	free(*dequeref); *dequeref = NULL;
}

static size_t _harbol_deque_alloc_node(struct HarbolDeque *const deque) {
	if( deque->freed==SIZE_MAX ) {
		return SIZE_MAX;
	}
	size_t const i = deque->freed;
	deque->freed = deque->prevs[i];
	if( deque->freed != SIZE_MAX && deque->freed < deque->cap ) {
		deque->nexts[deque->freed] = SIZE_MAX;
	}
	return i;
}

static void _harbol_deque_free_node(struct HarbolDeque *const deque, size_t const i) {
	if( i >= deque->cap ) {
		return;
	} else if( deque->freed==SIZE_MAX ) {
		deque->freed = i;
		deque->nexts[i] = deque->prevs[i] = SIZE_MAX;
	} else {
		deque->prevs[i] = deque->freed;
		deque->nexts[i] = SIZE_MAX;
		deque->nexts[deque->freed] = i;
		deque->freed = i;
	}
}

static bool _harbol_deque_resize(struct HarbolDeque *const deque, size_t const new_size) {
	if( !harbol_multi_recalloc(new_size, deque->cap, 3,
						&deque->nexts, sizeof *deque->nexts,
						&deque->prevs, sizeof *deque->prevs,
						&deque->datum, sizeof *deque->datum) ) {
		return false;
	}
	
	size_t const old_cap = deque->cap;
	deque->cap = new_size;
	for( size_t i=old_cap; i < deque->cap; i++ ) {
		deque->prevs[i] = ( i==old_cap )? deque->freed : i - 1;
		deque->nexts[i] = i + 1;
	}
	deque->nexts[deque->cap - 1] = SIZE_MAX;
	deque->freed = deque->cap - 1;
	return true;
}

HARBOL_EXPORT size_t harbol_deque_count(struct HarbolDeque const *const deque) {
	return deque->len;
}
HARBOL_EXPORT bool harbol_deque_empty(struct HarbolDeque const *const deque) {
	return( deque->datum==NULL || deque->head==SIZE_MAX || deque->len==0 );
}

HARBOL_EXPORT size_t harbol_deque_head(struct HarbolDeque const *const deque) {
	return deque->head;
}
HARBOL_EXPORT size_t harbol_deque_tail(struct HarbolDeque const *const deque) {
	return deque->tail;
}
HARBOL_EXPORT size_t harbol_deque_next(struct HarbolDeque const *const deque, size_t const node) {
	return( node >= deque->cap )? SIZE_MAX : deque->nexts[node];
}
HARBOL_EXPORT size_t harbol_deque_prev(struct HarbolDeque const *const deque, size_t const node) {
	return( node >= deque->cap )? SIZE_MAX : deque->prevs[node];
}

HARBOL_EXPORT void *harbol_deque_get_data(struct HarbolDeque const *const deque, size_t const node) {
	return( node >= deque->cap )? NULL : deque->datum[node];
}

HARBOL_EXPORT size_t harbol_deque_prepend(struct HarbolDeque *const restrict deque, void const *const val, size_t const datasize) {
	if( deque->len >= deque->cap && !_harbol_deque_resize(deque, deque->cap << 1) ) {
		return SIZE_MAX;
	}
	
	size_t const i = _harbol_deque_alloc_node(deque);
	if( i==SIZE_MAX ) {
		return SIZE_MAX;
	}
	
	deque->datum[i] = dup_data(val, datasize);
	if( deque->head==SIZE_MAX ) {
		deque->head = deque->tail = i;
	} else {
		deque->nexts[i] = deque->head;
		deque->prevs[i] = SIZE_MAX;
		deque->prevs[deque->head] = i;
		deque->head = i;
	}
	deque->len++;
	return i;
}

HARBOL_EXPORT size_t harbol_deque_append(struct HarbolDeque *const restrict deque, void const *const val, size_t const datasize) {
	if( deque->len >= deque->cap && !_harbol_deque_resize(deque, deque->cap << 1) ) {
		return SIZE_MAX;
	}
	
	size_t const i = _harbol_deque_alloc_node(deque);
	if( i==SIZE_MAX ) {
		return SIZE_MAX;
	}
	
	deque->datum[i] = dup_data(val, datasize);
	if( deque->tail==SIZE_MAX ) {
		deque->head = deque->tail = i;
	} else {
		deque->prevs[i] = deque->tail;
		deque->nexts[i] = SIZE_MAX;
		deque->nexts[deque->tail] = i;
		deque->tail = i;
	}
	deque->len++;
	return i;
}


HARBOL_EXPORT bool harbol_deque_pop_front(struct HarbolDeque *const restrict deque, void *const restrict val, size_t const datasize) {
	if( deque->head==SIZE_MAX ) {
		return false;
	}
	
	size_t const n = deque->head;
	deque->head = deque->nexts[n];
	deque->nexts[n] = deque->prevs[n] = SIZE_MAX;
	
	memcpy(val, deque->datum[n], datasize);
	free(deque->datum[n]); deque->datum[n] = NULL;
	
	_harbol_deque_free_node(deque, n);
	if( deque->head != SIZE_MAX ) {
		deque->prevs[deque->head] = SIZE_MAX;
	} else {
		deque->tail = SIZE_MAX;
	}
	deque->len--;
	return true;
}

HARBOL_EXPORT bool harbol_deque_pop_back(struct HarbolDeque *const restrict deque, void *const restrict val, size_t const datasize) {
	if( deque->tail==SIZE_MAX ) {
		return false;
	}
	
	size_t const n = deque->tail;
	deque->tail = deque->prevs[n];
	deque->nexts[n] = deque->prevs[n] = SIZE_MAX;
	
	memcpy(val, deque->datum[n], datasize);
	free(deque->datum[n]); deque->datum[n] = NULL;
	
	_harbol_deque_free_node(deque, n);
	if( deque->tail != SIZE_MAX ) {
		deque->nexts[deque->tail] = SIZE_MAX;
	} else {
		deque->head = SIZE_MAX;
	}
	deque->len--;
	return true;
}

HARBOL_EXPORT void *harbol_deque_get_front(struct HarbolDeque const *const deque) {
	return( deque->head==SIZE_MAX )? NULL : deque->datum[deque->head];
}
HARBOL_EXPORT void *harbol_deque_get_back(struct HarbolDeque const *const deque) {
	return( deque->tail==SIZE_MAX )? NULL : deque->datum[deque->tail];
}
