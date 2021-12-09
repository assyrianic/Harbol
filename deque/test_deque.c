#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "deque.h"

void test_harbol_deque(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_deque_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_deque(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_deque(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("deque :: test allocation/initialization.\n\n", debug_stream);
	struct HarbolDeque
		i = harbol_deque_make(8, &( bool ){false}),
		*p = harbol_deque_new(8)
	;
	
	fputs("deque :: test appending.\n\n", debug_stream);
	harbol_deque_append(&i, &( union Value ){1}, sizeof(union Value));
	harbol_deque_append(&i, &( union Value ){2}, sizeof(union Value));
	harbol_deque_append(&i, &( union Value ){3}, sizeof(union Value));
	harbol_deque_append(&i, &( union Value ){4}, sizeof(union Value));
	
	harbol_deque_append(p, &( union Value ){1}, sizeof(union Value));
	harbol_deque_append(p, &( union Value ){2}, sizeof(union Value));
	harbol_deque_append(p, &( union Value ){3}, sizeof(union Value));
	harbol_deque_append(p, &( union Value ){4}, sizeof(union Value));
	
	fputs("\ndeque :: iterating.\n", debug_stream);
	for( size_t n=harbol_deque_head(&i); n != SIZE_MAX; n = harbol_deque_next(&i, n) ) {
		const union Value *const val = harbol_deque_get_data(&i, n);
		fprintf(debug_stream, "index: %zu - value: %" PRIi64 "\n", n, val->int64);
	}
	
	fputs("deque :: test prepending.\n\n", debug_stream);
	harbol_deque_prepend(&i, &( union Value ){1}, sizeof(union Value));
	harbol_deque_prepend(&i, &( union Value ){2}, sizeof(union Value));
	harbol_deque_prepend(&i, &( union Value ){3}, sizeof(union Value));
	harbol_deque_prepend(&i, &( union Value ){4}, sizeof(union Value));
	
	harbol_deque_prepend(p, &( union Value ){1}, sizeof(union Value));
	harbol_deque_prepend(p, &( union Value ){2}, sizeof(union Value));
	harbol_deque_prepend(p, &( union Value ){3}, sizeof(union Value));
	harbol_deque_prepend(p, &( union Value ){4}, sizeof(union Value));
	
	fputs("\ndeque :: iterating.\n", debug_stream);
	for( size_t n=harbol_deque_head(&i); n != SIZE_MAX; n = harbol_deque_next(&i, n) ) {
		const union Value *const val = harbol_deque_get_data(&i, n);
		fprintf(debug_stream, "index: %zu - value: %" PRIi64 "\n", n, val->int64);
	}
	
	fprintf(debug_stream, "\ndeque :: node count: %zu.\n", harbol_deque_count(&i));
	
	fputs("\ndeque :: test popping values.\n", debug_stream);
	{
		union Value v = {0};
		harbol_deque_pop_front(&i, &v, sizeof v);
		fprintf(debug_stream, "popped front value: %" PRIi64 "\n", v.int64);
		
		harbol_deque_pop_back(&i, &v, sizeof v);
		fprintf(debug_stream, "popped back value: %" PRIi64 "\n", v.int64);
	}
	fputs("\ndeque :: iterating.\n", debug_stream);
	for( size_t n=harbol_deque_head(&i); n != SIZE_MAX; n = harbol_deque_next(&i, n) ) {
		const union Value *const val = harbol_deque_get_data(&i, n);
		fprintf(debug_stream, "index: %zu - value: %" PRIi64 "\n", n, val->int64);
	}
	
	fputs("\ndeque :: test reset.\n", debug_stream);
	harbol_deque_reset(&i);
	
	fputs("\ndeque :: iterating.\n", debug_stream);
	for( size_t n=harbol_deque_head(&i); n != SIZE_MAX; n = harbol_deque_next(&i, n) ) {
		const union Value *const val = harbol_deque_get_data(&i, n);
		fprintf(debug_stream, "index: %zu - value: %" PRIi64 "\n", n, val->int64);
	}
	
	/// free deque
	harbol_deque_clear(&i);
	fprintf(debug_stream, "i's item is null? '%s'\n", i.nodes != NULL ? "no" : "yes");
	
	harbol_deque_clear(p);
	fprintf(debug_stream, "p's item is null? '%s'\n", p->nodes != NULL ? "no" : "yes");
	harbol_deque_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n\n", p != NULL ? "no" : "yes");
}