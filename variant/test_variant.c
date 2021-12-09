#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "variant.h"

void test_harbol_variant(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_variant_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_variant(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_variant(FILE *const debug_stream)
{
	enum {
		TYPE_INT = 4,
		TYPE_FLOAT32,
		TYPE_FLOAT64
	};
	/// Test allocation and initializations
	fputs("variant :: test allocation / initialization.\n", debug_stream);
	struct HarbolVariant i = harbol_variant_make(&( int ){1}, sizeof(int), TYPE_INT, &( bool ){false});
	struct HarbolVariant *p = harbol_variant_new(&( int ){1}, sizeof(int), TYPE_INT);
	assert( p );
	
	fputs("\nvariant :: test retrieval.\n", debug_stream);
	fprintf(debug_stream, "i data == '%i'\n", *(( const int* )i.data));
	fprintf(debug_stream, "p data == '%i'\n", *(( const int* )p->data));
	
	fprintf(debug_stream, "i tag == '%i'\n", i.tag);
	fprintf(debug_stream, "p tag == '%i'\n", p->tag);
	
	fputs("\nvariant :: test setting.\n", debug_stream);
	harbol_variant_set(&i, &( int ){100}, sizeof(int));
	harbol_variant_set(p, &( int ){421}, sizeof(int));
	fprintf(debug_stream, "i data == '%i'\n", *(( const int* )i.data));
	fprintf(debug_stream, "p data == '%i'\n", *(( const int* )p->data));
	
	harbol_variant_set(&i, &( float32_t ){1.f}, sizeof(float32_t));
	i.tag = TYPE_FLOAT32;
	harbol_variant_set(p, &( float64_t ){3.4}, sizeof(float64_t));
	p->tag = TYPE_FLOAT64;
	fprintf(debug_stream, "i data == '%" PRIf32 "'\n", *(( const float32_t* )i.data));
	fprintf(debug_stream, "p data == '%" PRIf64 "'\n", *(( const float64_t* )p->data));
	
	/// free data
	fputs("\nvariant :: test destruction.\n", debug_stream);
	harbol_variant_clear(&i);
	fprintf(debug_stream, "i's data are null? '%s'\n", i.data != NULL ? "no" : "yes");
	
	harbol_variant_clear(p);
	fprintf(debug_stream, "p's data are null? '%s'\n", p->data != NULL ? "no" : "yes");
	harbol_variant_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}
