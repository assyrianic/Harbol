#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "mempool.h"

void test_harbol_mempool(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void) {
	FILE *debug_stream = fopen("harbol_mempool_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_mempool(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}

static NO_NULL void _mempool_print_bitmap(struct HarbolMemPool const *const mp, FILE *const debug_stream) {
	fprintf(debug_stream, "memory size: %zu | block size: %zu | bm_len: %zu\n", mp->mem_len, mp->blk_size, mp->bm_len);
	for( size_t i=0; i < mp->bm_len; i++ ) {
		if( mp->bitmap[i]==0 ) {
			continue;
		}
		harbol_print_bits(debug_stream, mp->bitmap[i], harbol_size_bits(sizeof *mp->bitmap), true);
	}
	fprintf(debug_stream, "\n");
}

void test_harbol_mempool(FILE *const debug_stream) {
	/// Test allocation and initializations
	fputs("mempool :: test allocation/initialization.\n", debug_stream);
	
	enum {
		MEMSIZE = 10000000, /// 10M
	};
	
	struct HarbolMemPool i = harbol_mempool_make(MEMSIZE, 128, LVAL_PTR(bool, false));
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	
	clock_t const start = clock();
	/// test giving memory
	fputs("mempool :: test giving memory.\n", debug_stream);
	fputs("\nmempool :: allocating int ptr.\n", debug_stream);
	int *p = harbol_mempool_alloc(&i, sizeof *p);
	fprintf(debug_stream, "p is null? '%s'\n", p != nullptr? "no" : "yes");
	if( p != nullptr ) {
		*p = 500;
		fprintf(debug_stream, "p's value: %i\n", *p);
	}
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	fputs("\nmempool :: allocating float ptr.\n", debug_stream);
	float *f = harbol_mempool_alloc(&i, sizeof *f);
	fprintf(debug_stream, "f is null? '%s'\n", f != nullptr? "no" : "yes");
	if( f != nullptr ) {
		*f = 500.5f;
		fprintf(debug_stream, "f's value: %f\n", *f);
	}
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	/// test releasing memory
	fputs("mempool :: test releasing memory.\n", debug_stream);
	harbol_mempool_free(&i, &f, sizeof *f);
	_mempool_print_bitmap(&i, debug_stream);
	harbol_mempool_free(&i, &p, sizeof *p);
	_mempool_print_bitmap(&i, debug_stream);
	
	/// test re-giving memory
	fputs("mempool :: test regiving memory.\n", debug_stream);
	p = harbol_mempool_alloc(&i, sizeof *p);
	fprintf(debug_stream, "p is null? '%s'\n", p != nullptr? "no" : "yes");
	if( p != nullptr ) {
		*p = 532;
		fprintf(debug_stream, "p's value: %i\n", *p);
	}
	
	f = harbol_mempool_alloc(&i, sizeof *f);
	fprintf(debug_stream, "f is null? '%s'\n", f != nullptr? "no" : "yes");
	if( f != nullptr ) {
		*f = 466.5f;
		fprintf(debug_stream, "f's value: %f\n", *f);
	}
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	harbol_mempool_free(&i, &p, sizeof *p); /// release memory that's from different region.
	harbol_mempool_free(&i, &f, sizeof *f);
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	/// test giving array memory
	fputs("\nmempool :: test giving array memory.\n", debug_stream);
	size_t const arrsize = 100;
	p = harbol_mempool_alloc(&i, sizeof *p * arrsize);
	fprintf(debug_stream, "p is null? '%s'\n", p != nullptr? "no" : "yes");
	if( p != nullptr ) {
		for( size_t i=0; i < arrsize; i++ ) {
			p[i] = i+1;
			fprintf(debug_stream, "p[%zu] value: %i\n", i, p[i]);
		}
	}
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	f = harbol_mempool_alloc(&i, sizeof *f * arrsize);
	fprintf(debug_stream, "f is null? '%s'\n", f != nullptr? "no" : "yes");
	if( f != nullptr ) {
		for( size_t i=0; i < arrsize; i++ ) {
			f[i] = i+1.15f;
			fprintf(debug_stream, "f[%zu] value: %f\n", i, f[i]);
		}
	}
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	harbol_mempool_free(&i, &p, sizeof *p * arrsize);
	harbol_mempool_free(&i, &f, sizeof *f * arrsize);
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	/// test using heap to make a unilinked list!
	fputs("\nmempool :: test using mempool for unilist.\n", debug_stream);
	struct LinkList {
		struct UniNode {
			uint8_t        *data;
			struct UniNode *next;
		} *head, *tail;
		size_t len;
	} *list = harbol_mempool_alloc(&i, sizeof *list);
	assert( list != nullptr && "list ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	struct UniNode *node1 = harbol_mempool_alloc(&i, sizeof *node1);
	assert( node1 != nullptr && "node1 is nullptr" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node1->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 1);
	{
		node1->next = nullptr;
		list->head = list->tail = node1;
	}
	
	struct UniNode *node2 = harbol_mempool_alloc(&i, sizeof *node2);
	assert( node2 != nullptr && "node2 ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node2->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 2);
	{
		node2->next = nullptr;
		list->tail = node2;
		list->head->next = node2;
	}
	
	struct UniNode *node3 = harbol_mempool_alloc(&i, sizeof *node3);
	assert( node3 != nullptr && "node3 ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node3->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 3);
	{
		node3->next = nullptr;
		list->tail->next = node3;
		list->tail = node3;
	}
	
	struct UniNode *node4 = harbol_mempool_alloc(&i, sizeof *node4);
	assert( node4 != nullptr && "node4 ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node4->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 4);
	{
		node4->next = nullptr;
		list->tail->next = node4;
		list->tail = node4;
	}
	
	struct UniNode *node5 = harbol_mempool_alloc(&i, sizeof *node5);
	assert( node5 != nullptr && "node5 ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node5->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 5);
	{
		node5->next = nullptr;
		list->tail->next = node5;
		list->tail = node5;
	}
	
	struct UniNode *node6 = harbol_mempool_alloc(&i, sizeof *node6);
	assert( node6 != nullptr && "node6 ptr is nullptr!" );
	fprintf(debug_stream, "remaining heap mem: '%zu'\n", harbol_mempool_remaining(&i, true));
	node6->data = ( uint8_t* ) LVAL_PTR(union Value, .int64 = 6);
	{
		node6->next = nullptr;
		list->tail->next = node6;
		list->tail = node6;
	}
	
	
	_mempool_print_bitmap(&i, debug_stream);
	for( struct UniNode *n = list->head; n != nullptr; n = n->next ) {
		fprintf(debug_stream, "uninode value : %" PRIi64 "\n", (( union Value const* )n->data)->int64);
	}
	harbol_mempool_free(&i, &list,  sizeof *list );
	harbol_mempool_free(&i, &node1, sizeof *node1);
	harbol_mempool_free(&i, &node2, sizeof *node2);
	harbol_mempool_free(&i, &node3, sizeof *node3);
	harbol_mempool_free(&i, &node4, sizeof *node4);
	harbol_mempool_free(&i, &node5, sizeof *node5);
	harbol_mempool_free(&i, &node6, sizeof *node6);
	_mempool_print_bitmap(&i, debug_stream);
	
	
	/// test "double freeing"
	fputs("\nmempool :: test double freeing.\n", debug_stream);
	p = harbol_mempool_alloc(&i, sizeof *p);
	fprintf(debug_stream, "p is null? '%s'\n", p? "no" : "yes");
	if( p != nullptr ) {
		*p = 500;
		fprintf(debug_stream, "p's value: %i\n", *p);
	}
	_mempool_print_bitmap(&i, debug_stream);
	harbol_mempool_free(&i, &p, sizeof *p);
	fprintf(debug_stream, "p is null? '%s'\n", p? "no" : "yes");
	_mempool_print_bitmap(&i, debug_stream);
	harbol_mempool_free(&i, &p, sizeof *p);
	fprintf(debug_stream, "p is null? '%s'\n", p? "no" : "yes");
	_mempool_print_bitmap(&i, debug_stream);
	fprintf(debug_stream, "\nmempool :: pool size == %zu.\n", harbol_mempool_remaining(&i, true));
	
	
	float *hk = harbol_mempool_alloc(&i, sizeof *hk * 99);
	double *fg = harbol_mempool_alloc(&i, sizeof *fg * 10);
	char *fff = harbol_mempool_alloc(&i, sizeof *fff * 50);
	float *f32 = harbol_mempool_alloc(&i, sizeof *f32 * 23);
	char *jj = harbol_mempool_alloc(&i, sizeof *jj * 100);
	
	_mempool_print_bitmap(&i, debug_stream);
	
	harbol_mempool_free(&i, &fff, sizeof *fff * 50);
	harbol_mempool_free(&i, &fg, sizeof *fg * 10);
	harbol_mempool_free(&i, &hk, sizeof *hk * 99);
	harbol_mempool_free(&i, &f32, sizeof *f32 * 23);
	fprintf(debug_stream, "\nmempool :: pool size == %zu.\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	harbol_mempool_free(&i, &jj, sizeof *jj * 100);
	fprintf(debug_stream, "\nmempool :: pool size == %zu.\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	
	fputs("\nmempool :: test reallocating jj to a single value.\n", debug_stream);
	jj = harbol_mempool_alloc(&i, sizeof *jj);
	*jj = 50;
	fprintf(debug_stream, "mempool :: jj == %i.\n", *jj);
	_mempool_print_bitmap(&i, debug_stream);
	
	int *newer = harbol_mempool_realloc(&i, &jj, sizeof *jj, sizeof *newer);
	fputs("\nmempool :: test reallocating jj to int ptr 'newer'.\n", debug_stream);
	fprintf(debug_stream, "mempool :: newer == %i.\n", *newer);
	_mempool_print_bitmap(&i, debug_stream);
	
	jj = harbol_mempool_realloc(&i, &newer, sizeof *newer, sizeof *jj);
	fputs("\nmempool :: test reallocating newer back to jj.\n", debug_stream);
	fprintf(debug_stream, "mempool :: jj == %i.\n", *jj);
	_mempool_print_bitmap(&i, debug_stream);
	
	newer = harbol_mempool_realloc(&i, &jj, sizeof *jj, sizeof *newer * 10);
	fputs("\nmempool :: test reallocating jj back to newer as an array of int[10].\n", debug_stream);
	for( size_t i=0; i < 10; i++ ) {
		newer[i] = i+1;
		fprintf(debug_stream, "mempool :: newer[%zu] == %i.\n", i, newer[i]);
	}
	fputs("\n", debug_stream);
	newer = harbol_mempool_realloc(&i, &newer, sizeof *newer * 10, sizeof *newer * 5);
	for( size_t i=0; i < 5; i++ ) {
		fprintf(debug_stream, "mempool :: reallocated newer[%zu] == %i.\n", i, newer[i]);
	}
	_mempool_print_bitmap(&i, debug_stream);
	harbol_mempool_free(&i, &newer, sizeof *newer * 5);
	
	fputs("\nmempool :: last bitmap check.\n", debug_stream);
	fprintf(debug_stream, "\nmempool :: pool size == %zu.\n", harbol_mempool_remaining(&i, true));
	_mempool_print_bitmap(&i, debug_stream);
	
	clock_t const end = clock();
	printf("memory pool run time: %f\n", (end-start)/( double )CLOCKS_PER_SEC);
	/// free data
	fputs("\nmempool :: test destruction.\n", debug_stream);
	harbol_mempool_clear(&i, false);
	fprintf(debug_stream, "i's heap is null? '%s'\n", i.mem != nullptr? "no" : "yes");
}