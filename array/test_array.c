#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "array.h"

void test_harbol_array(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_array_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_array(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_array(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("array :: test allocation/initialization.", debug_stream);
	fputs("\n", debug_stream);
	
	struct HarbolArray *p = harbol_array_new(sizeof(union Value), 4);
	assert( p );
	
	struct HarbolArray i = harbol_array_make(sizeof(union Value), 4, &( bool ){false});
	
	/// test data inserting
	fputs("array :: test insertion.", debug_stream);
	fputs("\n", debug_stream);
	harbol_array_insert(p, &( union Value ){.int64=100}, sizeof(union Value));
	harbol_array_insert(&i, &( union Value ){.int64=100}, sizeof(union Value));
	
	/// test data retrieval
	fputs("array :: test retrieval.", debug_stream);
	fputs("\n", debug_stream);
	fprintf(debug_stream, "ptr[0] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(p, 0, sizeof(union Value)))->int64);
	fprintf(debug_stream, "stk[0] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(&i, 0, sizeof(union Value)))->int64);
	
	/// test data setting
	fputs("array :: test setting data.", debug_stream);
	fputs("\n", debug_stream);
	harbol_array_set(p, 0, &( union Value ){.int64=10}, sizeof(union Value));
	harbol_array_set(&i, 0, &( union Value ){.int64=9}, sizeof(union Value));
	
	fprintf(debug_stream, "ptr[0] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(p, 0, sizeof(union Value)))->int64);
	fprintf(debug_stream, "stk[0] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(&i, 0, sizeof(union Value)))->int64);
	
	/// append the arrays
	fputs("array :: test array appending.", debug_stream);
	fputs("\n", debug_stream);
	harbol_array_add(p, &i, sizeof(union Value));
	fprintf(debug_stream, "ptr[1] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(p, 1, sizeof(union Value)))->int64);
	fprintf(debug_stream, "stk[1] == %p | len: %zu\n", harbol_array_get(&i, 1, sizeof(union Value)), i.len);
	
	/// test array copying.
	fputs("array :: test array copying.", debug_stream);
	fputs("\n", debug_stream);
	harbol_array_clear(&i);
	harbol_array_grow(&i, sizeof(union Value));
	
	harbol_array_insert(&i, &( union Value ){.int64=100}, sizeof(union Value));
	harbol_array_insert(&i, &( union Value ){.int64=101}, sizeof(union Value));
	harbol_array_insert(&i, &( union Value ){.int64=102}, sizeof(union Value));
	
	harbol_array_copy(p, &i, sizeof(union Value));
	fprintf(debug_stream, "stk[1] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(&i, 1, sizeof(union Value)))->int64);
	fprintf(debug_stream, "stk[2] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(&i, 2, sizeof(union Value)))->int64);
	fprintf(debug_stream, "ptr[1] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(p, 1, sizeof(union Value)))->int64);
	fprintf(debug_stream, "ptr[2] == %" PRIi64 "\n", (( const union Value* )harbol_array_get(p, 2, sizeof(union Value)))->int64);
	
	/// test array deleting and truncating.
	fputs("\narray :: test item deletion.\n", debug_stream);
	harbol_array_clear(&i); harbol_array_resize(&i, sizeof(union Value), 8);
	harbol_array_clear(p); harbol_array_resize(p, sizeof(union Value), 8);
	
	harbol_array_insert(p, &( union Value ){.int64=100}, sizeof(union Value));
	harbol_array_insert(p, &( union Value ){.int64=101}, sizeof(union Value));
	harbol_array_insert(p, &( union Value ){.int64=102}, sizeof(union Value));
	harbol_array_insert(p, &( union Value ){.int64=103}, sizeof(union Value));
	harbol_array_insert(p, &( union Value ){.int64=104}, sizeof(union Value));
	harbol_array_insert(p, &( union Value ){.int64=105}, sizeof(union Value));
	
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	fputs("\n", debug_stream);
	
	harbol_array_del_by_index(p, 0, sizeof(union Value)); /// deletes 100
	harbol_array_del_by_index(p, 1, sizeof(union Value)); /// deletes 102 since 101 because new 0 index
	harbol_array_del_by_index(p, 2, sizeof(union Value)); /// deletes 104
	
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	
	fputs("\narray :: test array truncation.\n", debug_stream);
	fprintf(debug_stream, "\nbefore truncating ptr[] cap == %zu\n", p->cap);
	
	harbol_array_shrink(p, sizeof(union Value), false);
	fprintf(debug_stream, "after truncating ptr[] cap == %zu\n\n", p->cap);
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	
	fputs("\narray :: test array popping.\n", debug_stream);
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "prepop ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	fputs("\n", debug_stream);
	const union Value *vec_item_2 = harbol_array_pop(p, sizeof(union Value));
	
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "postpop ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	fputs("\n", debug_stream);
	fprintf(debug_stream, "popped val == %" PRIi64 "\n", vec_item_2->int64);
	
	fputs("\narray :: test counting.\n", debug_stream);
	fprintf(debug_stream, "count of value '103' %zu\n", harbol_array_item_count(p, &( union Value ){.int64=103}, sizeof(union Value)));
	
	fputs("\narray :: test index of value.\n", debug_stream);
	fprintf(debug_stream, "index of value '101' %zu\n", harbol_array_index_of(p, &( union Value ){.int64=101}, sizeof(union Value), 0));
	fprintf(debug_stream, "count of value '101' %zu\n", harbol_array_item_count(p, &( union Value ){.int64=101}, sizeof(union Value)));
	
	fputs("\narray :: test reversing array.\n", debug_stream);
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "pre-reversing ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	
	harbol_array_swap(p, sizeof(union Value));
	fputs("\n", debug_stream);
	
	for( size_t i=0; i<p->len; i++ )
		fprintf(debug_stream, "post-reversing ptr[%zu] == %" PRIi64 "\n", i, (( const union Value* )harbol_array_get(p, i, sizeof(union Value)))->int64);
	
	
	/// free data
	fputs("\narray :: test destruction.\n", debug_stream);
	
	harbol_array_clear(&i);
	fprintf(debug_stream, "i's table is null? '%s'\n", i.table != NULL ? "no" : "yes");
	
	harbol_array_clear(p);
	fprintf(debug_stream, "p's table is null? '%s'\n", p->table != NULL ? "no" : "yes");
	harbol_array_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}