#ifndef HARBOL_INTRUSIVE_LISTS_INCLUDED
#	define HARBOL_INTRUSIVE_LISTS_INCLUDED

/**
 * Basic, Intrusive Linked List Headers.
 * Their usage is by embedding as the first member of a struct:
```c
struct item {
    struct HarbolLink links;
    int               value;
};

int main(void) {
	{
		struct item *a1 = &(struct item){.value = 1};
		
		harbol_link_add_to_head(&a1, &(struct item){.value = 2}, nullptr);
		harbol_link_add_to_head(&a1, &(struct item){.value = 3}, nullptr);
		harbol_link_add_to_head(&a1, &(struct item){.value = 4}, nullptr);
		harbol_link_add_to_head(&a1, &(struct item){.value = 5}, nullptr);
		
		int i = 1;
		for( struct item *n = a1; n != nullptr; n = harbol_link_get_a(n) ) {
		    printf("Item #%i | count: %i\n", n->value, i++);
		}
	}
	{
		struct item *a1 = &(struct item){.value = 1};
		
		harbol_bilink_add_to_head(&a1, &(struct item){.value = 2}, nullptr);
		harbol_bilink_add_to_head(&a1, &(struct item){.value = 4}, nullptr);
		harbol_bilink_add_to_head(&a1, &(struct item){.value = 3}, nullptr);
		harbol_bilink_add_to_head(&a1, &(struct item){.value = 5}, nullptr);
		
		int i = 1;
		struct item *last = nullptr;
		for( struct item *n = a1; n != nullptr; n = harbol_link_get_a(n) ) {
		    printf("Item #%i | count: %i\n", n->value, i++);
		    last = n;
		}
		puts("");
		i = 1;
		for( struct item *n = last; n != nullptr; n = harbol_bilink_get_b(n) ) {
		    printf("Item #%i | count: %i\n", n->value, i++);
		}
	}
}
```
 */
struct HarbolLink {
	struct HarbolLink *a; /// next/left in a linked list/tree.
};
HELPER_FUNC NO_NULL void *harbol_link_get_a(void const *const p) {
	struct HarbolLink const *const link = p;
	return link->a;
}
HELPER_FUNC NO_NULL void **harbol_link_get_a_ref(void *const p) {
	struct HarbolLink *const link = p;
	return ( void** )(&link->a);
}
HELPER_FUNC NEVER_NULL(1) void harbol_link_set_a(void *const p, void *const n) {
	struct HarbolLink *const link = p;
	link->a = n;
}

HELPER_FUNC NO_NULL void harbol_link_add_after(void *const n1, void *const n2) {
	struct HarbolLink
		*const restrict node1 = n1,
		*const          node2 = n2
	;
	node2->a = node1->a;
	node1->a = node2;
}

/// sometimes a linked list impl doesn't have/need a tail
/// but it at least always has a head/starting node.
HELPER_FUNC NEVER_NULL(1,2) void harbol_link_add_to_head(void *const h, void *const n, void *const t) {
	struct HarbolLink
		**const head = h,
		**const tail = t,
		 *const node = n
	;
	node->a = *head;
	*head = node;
	if( tail != nullptr && *tail==nullptr ) {
		*tail = node;
	}
}

/// for this, you absolutely need a tail.
HELPER_FUNC NO_NULL void harbol_link_add_to_tail(void *const h, void *const n, void *const t) {
	struct HarbolLink
		**const head = h,
		**const tail = t,
		 *const node = n
	;
	if( *head != nullptr ) {
		node->a = nullptr;
		(*tail)->a = node;
		*tail = node;
	} else {
		*head = node;
		*tail = node;
	}
}


/// use `harbol_link_` for 'a' member.
struct HarbolBiLink {
	struct HarbolBiLink *a, *b;
};
HELPER_FUNC NO_NULL void *harbol_bilink_get_b(void const *const p) {
	struct HarbolBiLink const *const link = p;
	return link->b;
}
HELPER_FUNC NO_NULL void **harbol_bilink_get_b_ref(void *const p) {
	struct HarbolBiLink *const link = p;
	return ( void** )(&link->b);
}
HELPER_FUNC NEVER_NULL(1) void harbol_bilink_set_b(void *const p, void *const n) {
	struct HarbolBiLink *const link = p;
	link->b = n;
}

HELPER_FUNC NO_NULL void harbol_bilink_add_after(void *const n1, void *const n2) {
	struct HarbolBiLink *const node1 = n1;
	struct HarbolBiLink *const node2 = n2;
	node2->b = node1;
	if( node1->a != nullptr ) {
		node1->a->b = node2;
	}
	node2->a = node1->a;
	node1->a = node2;
}
HELPER_FUNC NEVER_NULL(1,2) void harbol_bilink_add_to_head(void *const h, void *const n, void *const t) {
	struct HarbolBiLink
		**const head = h,
		**const tail = t,
		 *const node = n
	;
	if( *head==nullptr ) {
		*head = node;
		if( tail != nullptr ) {
			*tail = node;
		}
		node->a = node->b = nullptr;
	} else {
		node->a = *head;
		node->b = nullptr;
		(*head)->b = node;
		*head = node;
	}
}
HELPER_FUNC NO_NULL void harbol_bilink_add_to_tail(void *const h, void *const n, void *const t) {
	struct HarbolBiLink
		**const head = h,
		**const tail = t,
		 *const node = n
	;
	if( *tail==nullptr ) {
		*head = node;
		*tail = node;
		node->a = node->b = nullptr;
	} else {
		node->b = *tail;
		node->a = nullptr;
		(*tail)->a = node;
		*tail = node;
	}
}
HELPER_FUNC NO_NULL void harbol_bilink_remove(void *const h, void *const n, void *const t) {
	struct HarbolBiLink
		**const head = h,
		**const tail = t,
		 *const node = n
	;
	if( node->b != nullptr ) {
		node->b->a = node->a;
	} else {
		*head = node->a;
		if( *head != nullptr ) {
			(*head)->b = nullptr;
		} else {
			*tail = nullptr;
		}
	}
	
	if( node->a != nullptr ) {
		node->a->b = node->b;
	} else {
		*tail = node->b;
		if( *tail != nullptr ) {
			(*tail)->a = nullptr;
		} else {
			*head = nullptr;
		}
	}
}

/// use `harbol_link_` for 'a' member & `harbol_bilink_` for 'b' member.
struct HarbolTriLink {
	struct HarbolTriLink *a, *b, *c;
};
HELPER_FUNC NO_NULL void *harbol_trilink_get_c(void const *const p) {
	struct HarbolTriLink const *const link = p;
	return link->c;
}
HELPER_FUNC NO_NULL void **harbol_trilink_get_c_ref(void *const p) {
	struct HarbolTriLink *const link = p;
	return ( void** )(&link->c);
}
HELPER_FUNC NEVER_NULL(1) void harbol_trilink_set_c(void *const p, void *const n) {
	struct HarbolTriLink *const link = p;
	link->c = n;
}

#endif /** HARBOL_INTRUSIVE_LISTS_INCLUDED */