#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "map.h"

void test_harbol_map(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_map_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_map(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_map(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("map :: test allocation / initialization.\n", debug_stream);
	struct HarbolMap i = harbol_map_make(8, &( bool ){false});
	struct HarbolMap *p = harbol_map_new(8);
	assert( p );
	
	/// test insertion
	fputs("\nmap :: test insertion.\n", debug_stream);
	harbol_map_insert(p, "1", 2, &( union Value ){.int64=1}, sizeof(union Value));
	harbol_map_insert(p, "2", 2, &( union Value ){.int64=2}, sizeof(union Value));
	harbol_map_insert(p, "3", 2, &( union Value ){.int64=3}, sizeof(union Value));
	harbol_map_insert(p, "4", 2, &( union Value ){.int64=4}, sizeof(union Value));
	harbol_map_insert(p, "5", 2, &( union Value ){.int64=5}, sizeof(union Value));
	harbol_map_insert(p, "6", 2, &( union Value ){.int64=6}, sizeof(union Value));
	harbol_map_insert(p, "7", 2, &( union Value ){.int64=7}, sizeof(union Value));
	harbol_map_insert(p, "8", 2, &( union Value ){.int64=8}, sizeof(union Value));
	harbol_map_insert(p, "9", 2, &( union Value ){.int64=9}, sizeof(union Value));
	harbol_map_insert(p, "10", 3, &( union Value ){.int64=10}, sizeof(union Value));
	
	harbol_map_insert(&i, "1", 2, &( union Value ){.int64=1}, sizeof(union Value));
	harbol_map_insert(&i, "2", 2, &( union Value ){.int64=2}, sizeof(union Value));
	harbol_map_insert(&i, "3", 2, &( union Value ){.int64=3}, sizeof(union Value));
	harbol_map_insert(&i, "4", 2, &( union Value ){.int64=4}, sizeof(union Value));
	harbol_map_insert(&i, "5", 2, &( union Value ){.int64=5}, sizeof(union Value));
	harbol_map_insert(&i, "6", 2, &( union Value ){.int64=6}, sizeof(union Value));
	harbol_map_insert(&i, "7", 2, &( union Value ){.int64=7}, sizeof(union Value));
	harbol_map_insert(&i, "8", 2, &( union Value ){.int64=8}, sizeof(union Value));
	harbol_map_insert(&i, "9", 2, &( union Value ){.int64=9}, sizeof(union Value));
	harbol_map_insert(&i, "10", 3, &( union Value ){.int64=10}, sizeof(union Value));

	fputs("\nmap :: looping through all data.\n", debug_stream);
	for( size_t n=0; n<p->len; n++ ) {
		fprintf(debug_stream, "key: '%s', keylen: '%zu' - value == %" PRIi64 "\n", p->keys[n], p->keylens[n], (( const union Value* )p->datum[n])->int64);
	}
	
	/// test retrieval.
	fputs("\nmap :: test data retrieval.\n", debug_stream);
	fprintf(debug_stream, "ptr[\"1\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(p, "1", 2))->int64);
	fprintf(debug_stream, "ptr[\"2\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(p, "2", 2))->int64);
	fprintf(debug_stream, "stk[\"1\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(&i, "1", 2))->int64);
	fprintf(debug_stream, "stk[\"2\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(&i, "2", 2))->int64);
	
	fputs("\nmap :: looping through all data.\n", debug_stream);
	for( size_t n=0; n<i.len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", i.keys[n], (( const union Value* )i.datum[n])->int64);
	}
	/// test setting.
	fputs("\nmap :: test data setting.\n", debug_stream);
	harbol_map_key_set(p, "2", 2, &( union Value ){.int64=20}, sizeof(union Value));
	harbol_map_key_set(&i, "2", 2, &( union Value ){.int64=200}, sizeof(union Value));
	fprintf(debug_stream, "ptr[\"1\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(p, "1", 2))->int64);
	fprintf(debug_stream, "ptr[\"2\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(p, "2", 2))->int64);
	fprintf(debug_stream, "stk[\"1\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(&i, "1", 2))->int64);
	fprintf(debug_stream, "stk[\"2\"] == %" PRIi64 "\n", (( const union Value* )harbol_map_key_get(&i, "2", 2))->int64);
	fputs("\nmap :: looping through all data.\n", debug_stream);
	for( size_t n=0; n<i.len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", i.keys[n], (( const union Value* )i.datum[n])->int64);
	}
	fputs("\n", debug_stream);
	for( size_t n=0; n<p->len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", p->keys[n], (( const union Value* )p->datum[n])->int64);
	}
	
	/// test deletion
	fputs("\nmap :: test item deletion.\n", debug_stream);
	harbol_map_key_rm(p, "2", 1);
	fprintf(debug_stream, "ptr[\"2\"] == %p\n", harbol_map_key_get(p, "2", 2));
	fputs("\nmap :: looping through all data.\n", debug_stream);
	for( size_t n=0; n<p->len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", p->keys[n], (( const union Value* )p->datum[n])->int64);
	}
	fputs("\nmap :: test item deletion by index.\n", debug_stream);
	harbol_map_idx_rm(p, 2);
	for( size_t n=0; n<p->len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", p->keys[n], (( const union Value* )p->datum[n])->int64);
	}
	/// test setting by index
	fputs("\nmap :: test item setting by index.\n", debug_stream);
	harbol_map_idx_set(p, 2, &( union Value ){.int64=500}, sizeof(union Value));
	for( size_t n=0; n<p->len; n++ ) {
		fprintf(debug_stream, "key '%s' - value == %" PRIi64 "\n", p->keys[n], (( const union Value* )p->datum[n])->int64);
	}
	/// free data
	fputs("\nmap :: test destruction.\n", debug_stream);
	harbol_map_clear(&i);
	fprintf(debug_stream, "i's buckets are null? '%s'\n", i.buckets != NULL ? "no" : "yes");
	
	harbol_map_clear(p);
	fprintf(debug_stream, "p's buckets are null? '%s'\n", p->buckets != NULL ? "no" : "yes");
	harbol_map_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}
