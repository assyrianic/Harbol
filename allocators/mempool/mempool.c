#include "mempool.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolMemNode *harbol_memnode_split(struct HarbolMemNode *const node, size_t const bytes) {
	uintptr_t const n = ( uintptr_t )(node);
	struct HarbolMemNode *const r = ( struct HarbolMemNode* )(n + (node->size - bytes));
	node->size -= bytes;
	r->size = bytes;
	return r;
}


HARBOL_EXPORT void harbol_memnode_replace(struct HarbolMemNode *const old, struct HarbolMemNode *const replace) {
	/// replace this node.
	replace->prev = old->prev;
	replace->next = old->next;
	if( replace->prev != NULL )
		replace->prev->next = replace;
	if( replace->next != NULL )
		replace->next->prev = replace;
}


/// makes a current node into the next node.
HARBOL_EXPORT void harbol_freelist_insert_before(struct HarbolFreeList *const list, struct HarbolMemNode *const curr, struct HarbolMemNode *const insert) {
	insert->next = curr;
	if( curr->prev==NULL ) {
		list->head = insert;
	} else {
		insert->prev = curr->prev;
		curr->prev->next = insert;
	}
    curr->prev = insert;
}

static void _harbol_freelist_transfer_node(struct HarbolMemPool *const mempool, struct HarbolMemNode *const node) {
	size_t const slot = (node->size >> HARBOL_BUCKET_BITS) - 1;
	struct HarbolFreeList *const l = (slot < HARBOL_BUCKET_SIZE)? &mempool->buckets[slot] : &mempool->large;
	harbol_freelist_insert(mempool, l, node, (slot < HARBOL_BUCKET_SIZE));
}


HARBOL_EXPORT void harbol_freelist_insert(struct HarbolMemPool *const mempool, struct HarbolFreeList *const list, struct HarbolMemNode *const node, bool const is_bucket) {
	if( list->head==NULL ) {
		list->head = node;
		list->len++;
		return;
	}
	
	for( struct HarbolMemNode *iter = list->head; iter != NULL; iter = iter->next ) {
		if( ( uintptr_t )(iter)==mempool->stack.offs ) {
			mempool->stack.offs += iter->size;
			harbol_freelist_remove(list, iter);
			iter = list->head;
			if( iter==NULL ) {
				list->head = node;    
				return;
            }
			continue;
		}
		uintptr_t const inode = ( uintptr_t )(node);
		uintptr_t const iiter = ( uintptr_t )(iter);
		uintptr_t const iter_end = iiter + iter->size;
		uintptr_t const node_end = inode + node->size;
		
		if( iter==node ) {
			return;
		} else if( iter < node ) {
			if( iter_end > inode ) {
				/// node was coalesced prior.
				return;
			} else if( iter_end==inode ) {
				/// if we can coalesce, do so.
				iter->size += node->size;
				return;
			} else if (iter->next == NULL) {
				// we reached the end of the free list -> append the node
				iter->next = node;
				node->prev = iter;
				list->len++;
				return;    
            }
		} else if( iter > node ) {
			/// Address sort, lowest to highest aka ascending order.
			if( iter==list->head ) {
				if( iter_end==inode ) {
					iter->size += node->size;
					if( is_bucket ) {
						_harbol_freelist_transfer_node(mempool, harbol_freelist_remove(list, iter));
					}
				} else if( node_end==iiter ) {
					node->size += list->head->size;
					node->next = list->head->next;
					node->prev = NULL;
					list->head = node;
					if( is_bucket ) {
						_harbol_freelist_transfer_node(mempool, harbol_freelist_remove(list, list->head));
					}
				} else {
					node->next = iter;
					node->prev = NULL;
					iter->prev = node;
					list->head = node;
					list->len++;
				}
				return;
			} else if( iter_end==inode ) {
				iter->size += node->size;
				if( is_bucket ) {
					_harbol_freelist_transfer_node(mempool, harbol_freelist_remove(list, iter));
				}
				return;
			} else {
				harbol_freelist_insert_before(list, node, iter);
				list->len++;
				return;
			}
		}
	}
}

HARBOL_EXPORT struct HarbolMemNode *harbol_freelist_remove(struct HarbolFreeList *const list, struct HarbolMemNode *const node) {
	if( node->prev != NULL ) {
		node->prev->next = node->next;
	} else {
		list->head = node->next;
		if( list->head != NULL )
			list->head->prev = NULL;
		else list->tail = NULL;
	}
	
	if( node->next != NULL ) {
		node->next->prev = node->prev;
	} else {
		list->tail = node->prev;
		if( list->tail != NULL )
			list->tail->next = NULL;
		else list->head = NULL;
	}
	list->len--;
	return node;
}

HARBOL_EXPORT struct HarbolMemNode *harbol_freelist_find(struct HarbolFreeList *const list, size_t const bytes) {
	for( struct HarbolMemNode *node = list->head; node != NULL; node = node->next ) {
		if( node->size < bytes ) {
			continue;
		} else if( node->size <= bytes + MEM_SPLIT_THRESHOLD ) {
			/// close in size - reduce fragmentation by not splitting.
			return harbol_freelist_remove(list, node);
		} else {
			return harbol_memnode_split(node, bytes);
		}
	}
	return NULL;
}


HARBOL_EXPORT bool harbol_mempool_init(struct HarbolMemPool *const mempool, size_t const size) {
	if( size==0 )
		return false;
	
	mempool->stack = harbol_region_make(size);
	return true;
}

HARBOL_EXPORT struct HarbolMemPool harbol_mempool_make(size_t const size, bool *const res) {
	struct HarbolMemPool mempool = {0};
	*res = harbol_mempool_init(&mempool, size);
	return mempool;
}

HARBOL_EXPORT bool harbol_mempool_init_from_buffer(struct HarbolMemPool *const mempool, void *const restrict buf, size_t const size) {
	if( size==0 || size<=sizeof(struct HarbolMemNode) )
		return false;
	
	mempool->stack = harbol_region_make_from_buffer(buf, size);
	return true;
}

HARBOL_EXPORT struct HarbolMemPool harbol_mempool_make_from_buffer(void *const restrict buf, size_t const size, bool *const restrict res) {
	struct HarbolMemPool mempool = { 0 };
	*res = harbol_mempool_init_from_buffer(&mempool, buf, size);
	return mempool;
}

HARBOL_EXPORT void harbol_mempool_clear(struct HarbolMemPool *const mempool) {
	harbol_region_clear(&mempool->stack);
	*mempool = ( struct HarbolMemPool ){0};
}

static NO_NULL void *_get_freenode(struct HarbolMemPool *const mempool, size_t const bytes) {
	/// check if we have a good sized node from the buckets.
	size_t const slot = (bytes >> HARBOL_BUCKET_BITS) - 1;
	struct HarbolFreeList *const l = (slot < HARBOL_BUCKET_SIZE)? &mempool->buckets[slot] : &mempool->large;
	struct HarbolMemNode *new_mem = harbol_freelist_find(l, bytes);
	if( new_mem==NULL ) {
		return NULL;
	} else {
		new_mem->next = new_mem->prev = NULL;
		uint8_t *const final_mem = ( uint8_t* )(new_mem) + sizeof *new_mem;
		return memset(final_mem, 0, new_mem->size - sizeof *new_mem);
	}
}

static NO_NULL void *_get_stknode(struct HarbolMemPool *const mempool, size_t const alloc_bytes) {
	struct HarbolMemNode *new_mem = harbol_region_alloc(&mempool->stack, alloc_bytes);
	if( new_mem==NULL ) {
		return NULL;
	} else {
		new_mem->size = alloc_bytes;
		new_mem->next = new_mem->prev = NULL;
		uint8_t *const final_mem = ( uint8_t* )(new_mem) + sizeof *new_mem;
		return memset(final_mem, 0, new_mem->size - sizeof *new_mem);
	}
}

HARBOL_EXPORT void *harbol_mempool_alloc(struct HarbolMemPool *const mempool, size_t const size) {
	if( size==0 || size > mempool->stack.size ) {
		return NULL;
	} else {
		/// visual of the allocation block.
		/// --------------
		/// |  mem size  | lowest addr of block
		/// |  next node | 12 bytes - 32 bit
		/// |  prev node | 24 bytes - 64 bit
		/// |------------|
		/// |   alloc'd  |
		/// |   memory   |
		/// |   space    | highest addr of block
		/// --------------
		size_t const alloc_bytes = harbol_align_size(size + sizeof(struct HarbolMemNode), sizeof(intptr_t));
		void *restrict new_mem = _get_freenode(mempool, alloc_bytes);
		return ( new_mem==NULL )? _get_stknode(mempool, alloc_bytes) : new_mem;
	}
}

HARBOL_EXPORT void *harbol_mempool_realloc(struct HarbolMemPool *const restrict mempool, void *const ptr, size_t const size) {
	if( size > mempool->stack.size ) {
		return NULL;
	} else if( ptr==NULL ) {
		/// NULL ptr should make this work like regular alloc.
		return harbol_mempool_alloc(mempool, size);
	} else if( ( uintptr_t )(ptr) - sizeof(struct HarbolMemNode) < ( uintptr_t )(mempool->stack.mem) ) {
		return NULL;
	}
	
	struct HarbolMemNode *node = ( struct HarbolMemNode* )(( uint8_t* )(ptr) - sizeof *node);
	uint8_t *resized_block = harbol_mempool_alloc(mempool, size);
	if( resized_block==NULL ) {
		return NULL;
	}
	struct HarbolMemNode *resized = ( struct HarbolMemNode* )(resized_block - sizeof *resized);
	memmove(resized_block, ptr, ((node->size > resized->size)? (resized->size) : (node->size)) - sizeof *node);
	harbol_mempool_free(mempool, ptr);
	return resized_block;
}

HARBOL_EXPORT bool harbol_mempool_free(struct HarbolMemPool *const restrict mempool, void *const ptr) {
	if( ptr==NULL || ( uintptr_t )(ptr) - sizeof(struct HarbolMemNode) < ( uintptr_t )(mempool->stack.mem) ) {
		return false;
	}
	/// behind the actual pointer data is the allocation info.
	struct HarbolMemNode *mem_node = ( struct HarbolMemNode* )(( uint8_t* )(ptr) - sizeof *mem_node);
	size_t const slot = (mem_node->size >> HARBOL_BUCKET_BITS) - 1;
	
	/// make sure the pointer data is valid.
	if( !is_uintptr_in_bounds(( uintptr_t )(mem_node), ( uintptr_t )(mempool->stack.mem + mempool->stack.size), ( uintptr_t )(mempool->stack.offs))
			|| !is_uint_in_bounds(mem_node->size, mempool->stack.size, sizeof(struct HarbolMemNode)) )
		return false;
	
	/// if the mem_node is right at the stack base ptr, then add it to the stack.
	if( ( uintptr_t )(mem_node)==mempool->stack.offs ) {
		mempool->stack.offs += mem_node->size;
	} else {
		/// try to place it into bucket or large freelist.
		struct HarbolFreeList *const list = ( slot < HARBOL_BUCKET_SIZE )? &mempool->buckets[slot] : &mempool->large;
		harbol_freelist_insert(mempool, list, mem_node, ( slot < HARBOL_BUCKET_SIZE ));
	}
	return true;
}

HARBOL_EXPORT bool harbol_mempool_cleanup(struct HarbolMemPool *const restrict mempool, void **const restrict ptrref) {
	if( *ptrref==NULL ) {
		return false;
	}
	bool const free_result = harbol_mempool_free(mempool, *ptrref);
	*ptrref = NULL;
	return free_result;
}

HARBOL_EXPORT size_t harbol_mempool_mem_remaining(struct HarbolMemPool const *mempool) {
	size_t total_remaining = mempool->stack.mem - (mempool->stack.mem + mempool->stack.offs);
	for( struct HarbolMemNode *n = mempool->large.head; n != NULL; n = n->next ) {
		total_remaining += n->size;
	}
	for( size_t i=0; i < HARBOL_BUCKET_SIZE; i++ ) {
		for( struct HarbolMemNode *n=mempool->buckets[i].head; n != NULL; n = n->next ) {
			total_remaining += n->size;
		}
	}
	return total_remaining;
}
