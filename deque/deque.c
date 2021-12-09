#include "deque.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

HARBOL_EXPORT struct HarbolDeque *harbol_deque_new(const size_t init_size) {
	struct HarbolDeque *deque = calloc(1, sizeof *deque);
	if( deque==NULL || !harbol_deque_init(deque, init_size) ) {
		free(deque);
		return NULL;
	}
	return deque;
}

HARBOL_EXPORT bool harbol_deque_init(struct HarbolDeque *const deque, const size_t init_size) {
	deque->nodes = harbol_recalloc(deque->nodes, init_size, sizeof *deque->nodes, deque->cap);
	if( deque->nodes==NULL )
		return false;
	
	deque->cap = init_size;
	for( size_t i=0; i<init_size; i++ ) {
		deque->nodes[i].prev = ( i==0 ) ? SIZE_MAX : i - 1;
		deque->nodes[i].next = i + 1;
	}
	deque->head = deque->tail = deque->nodes[init_size - 1].next = SIZE_MAX;
	deque->freed = init_size - 1;
	return true;
}

HARBOL_EXPORT struct HarbolDeque harbol_deque_make(const size_t init_size, bool *const res) {
	struct HarbolDeque d = {0};
	*res = harbol_deque_init(&d, init_size);
	return d;
}

HARBOL_EXPORT void harbol_deque_reset(struct HarbolDeque *const deque) {
	for( size_t i=0; i<deque->cap; i++ ) {
		free(deque->nodes[i].data); deque->nodes[i].data = NULL;
		deque->nodes[i].prev = ( i==0 ) ? SIZE_MAX : i - 1;
		deque->nodes[i].next = i + 1;
	}
	deque->head = deque->tail = deque->nodes[deque->cap - 1].next = SIZE_MAX;
	deque->freed = deque->cap - 1;
}

HARBOL_EXPORT void harbol_deque_clear(struct HarbolDeque *const deque) {
	for( size_t i=0; i<deque->cap; i++ ) {
		free(deque->nodes[i].data); deque->nodes[i].data = NULL;
	}
	free(deque->nodes); deque->nodes = NULL;
	*deque = ( struct HarbolDeque ){0};
}
HARBOL_EXPORT void harbol_deque_free(struct HarbolDeque **const dequeref) {
	if( *dequeref==NULL )
		return;
	
	harbol_deque_clear(*dequeref);
	free(*dequeref); *dequeref = NULL;
}

static size_t _harbol_deque_alloc_node(struct HarbolDeque *const deque) {
	if( deque->freed==SIZE_MAX )
		return SIZE_MAX;
	
	const size_t i = deque->freed;
	deque->freed = deque->nodes[i].prev;
	if( deque->freed != SIZE_MAX && deque->freed<deque->cap )
		deque->nodes[deque->freed].next = SIZE_MAX;
	
	return i;
}

static void _harbol_deque_free_node(struct HarbolDeque *const deque, const size_t i) {
	if( i>=deque->cap ) {
		return;
	} else if( deque->freed==SIZE_MAX ) {
		deque->freed = i;
		deque->nodes[i].next = deque->nodes[i].prev = SIZE_MAX;
	} else {
		deque->nodes[i].prev = deque->freed;
		deque->nodes[i].next = SIZE_MAX;
		deque->nodes[deque->freed].next = i;
		deque->freed = i;
	}
}

static bool _harbol_deque_resize(struct HarbolDeque *const deque, const size_t new_size) {
	struct HarbolQNode *const more_nodes = harbol_recalloc(deque->nodes, new_size, sizeof *deque->nodes, deque->cap);
	if( more_nodes==NULL )
		return false;
	
	deque->nodes = more_nodes;
	const size_t old_cap = deque->cap;
	deque->cap = new_size;
	for( size_t i=old_cap; i<deque->cap; i++ ) {
		deque->nodes[i].prev = ( i==old_cap ) ? deque->freed : i - 1;
		deque->nodes[i].next = i + 1;
	}
	deque->nodes[deque->cap - 1].next = SIZE_MAX;
	deque->freed = deque->cap - 1;
	return true;
}

HARBOL_EXPORT size_t harbol_deque_count(const struct HarbolDeque *const deque) {
	return deque->len;
}
HARBOL_EXPORT bool harbol_deque_empty(const struct HarbolDeque *const deque) {
	return( deque->nodes==NULL || deque->head==SIZE_MAX || deque->len==0 );
}

HARBOL_EXPORT size_t harbol_deque_head(const struct HarbolDeque *const deque) {
	return deque->head;
}
HARBOL_EXPORT size_t harbol_deque_tail(const struct HarbolDeque *const deque) {
	return deque->tail;
}
HARBOL_EXPORT size_t harbol_deque_next(const struct HarbolDeque *const deque, const size_t node) {
	return( node >= deque->cap ) ? SIZE_MAX : deque->nodes[node].next;
}
HARBOL_EXPORT size_t harbol_deque_prev(const struct HarbolDeque *const deque, const size_t node) {
	return( node >= deque->cap ) ? SIZE_MAX : deque->nodes[node].prev;
}

HARBOL_EXPORT void *harbol_deque_get_data(const struct HarbolDeque *const deque, const size_t node) {
	return( node >= deque->cap ) ? NULL : deque->nodes[node].data;
}

HARBOL_EXPORT size_t harbol_deque_prepend(struct HarbolDeque *const restrict deque, const void *const val, const size_t datasize) {
	if( deque->len >= deque->cap && !_harbol_deque_resize(deque, deque->cap << 1) )
		return SIZE_MAX;
	
	const size_t i = _harbol_deque_alloc_node(deque);
	if( i==SIZE_MAX )
		return SIZE_MAX;
	
	deque->nodes[i].data = dup_data(val, datasize);
	if( deque->head==SIZE_MAX ) {
		deque->head = deque->tail = i;
	} else {
		deque->nodes[i].next = deque->head;
		deque->nodes[i].prev = SIZE_MAX;
		deque->nodes[deque->head].prev = i;
		deque->head = i;
	}
	deque->len++;
	return i;
}
HARBOL_EXPORT size_t harbol_deque_append(struct HarbolDeque *const restrict deque, const void *const val, const size_t datasize) {
	if( deque->len >= deque->cap && !_harbol_deque_resize(deque, deque->cap << 1) )
		return SIZE_MAX;
	
	const size_t i = _harbol_deque_alloc_node(deque);
	if( i==SIZE_MAX )
		return SIZE_MAX;
	
	deque->nodes[i].data = dup_data(val, datasize);
	if( deque->tail==SIZE_MAX ) {
		deque->head = deque->tail = i;
	} else {
		deque->nodes[i].prev = deque->tail;
		deque->nodes[i].next = SIZE_MAX;
		deque->nodes[deque->tail].next = i;
		deque->tail = i;
	}
	deque->len++;
	return i;
}


HARBOL_EXPORT bool harbol_deque_pop_front(struct HarbolDeque *const restrict deque, void *const restrict val, const size_t datasize) {
	if( deque->head==SIZE_MAX )
		return false;
	
	const size_t n = deque->head;
	deque->head = deque->nodes[n].next;
	deque->nodes[n].next = deque->nodes[n].prev = SIZE_MAX;
	
	memcpy(val, deque->nodes[n].data, datasize);
	free(deque->nodes[n].data); deque->nodes[n].data = NULL;
	
	_harbol_deque_free_node(deque, n);
	
	if( deque->head != SIZE_MAX ) {
		deque->nodes[deque->head].prev = SIZE_MAX;
	} else {
		deque->tail = SIZE_MAX;
	}
	deque->len--;
	return true;
}
HARBOL_EXPORT bool harbol_deque_pop_back(struct HarbolDeque *const restrict deque, void *const restrict val, const size_t datasize) {
	if( deque->tail==SIZE_MAX )
		return false;
	
	const size_t n = deque->tail;
	deque->tail = deque->nodes[n].prev;
	deque->nodes[n].next = deque->nodes[n].prev = SIZE_MAX;
	
	memcpy(val, deque->nodes[n].data, datasize);
	free(deque->nodes[n].data); deque->nodes[n].data = NULL;
	
	_harbol_deque_free_node(deque, n);
	
	if( deque->tail != SIZE_MAX ) {
		deque->nodes[deque->tail].next = SIZE_MAX;
	} else {
		deque->head = SIZE_MAX;
	}
	deque->len--;
	return true;
}

HARBOL_EXPORT void *harbol_deque_get_front(const struct HarbolDeque *const deque) {
	return( deque->head==SIZE_MAX ) ? NULL : deque->nodes[deque->head].data;
}
HARBOL_EXPORT void *harbol_deque_get_back(const struct HarbolDeque *const deque) {
	return( deque->tail==SIZE_MAX ) ? NULL : deque->nodes[deque->tail].data;
}
