#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "tuple.h"

void test_harbol_tuple(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_tuple_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_tuple(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_tuple(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("tuple :: test allocation/initialization.\n", debug_stream);
	fputs("\n", debug_stream);
	
	const size_t struc[] = { sizeof(char), sizeof(int), sizeof(short) };
	
	struct HarbolTuple *p = harbol_tuple_new(1[&struc] - 0[&struc], struc, false);
	assert( p );
	fprintf(debug_stream, "p's size: '%zu'\n", p->len);
	
	fputs("\ntuple :: printing 3-tuple fields.\n", debug_stream);
	for( size_t i=0; i<p->fields.len; i++ ) {
		const struct {
			uint16_t offset, size;
		} *info = harbol_array_get(&p->fields, i, sizeof *info);
		fprintf(debug_stream, "index: %zu -> p's offset: '%u' | size: '%u'\n", i, info->offset, info->size);
	}
	fputs("\n", debug_stream);
	
	harbol_tuple_free(&p);
	
	p = harbol_tuple_new(1[&struc] - 0[&struc], struc, true);
	assert( p );
	fprintf(debug_stream, "packed p's size: '%zu'\n", p->len);
	
	fputs("\ntuple :: printing packed 3-tuple fields.\n", debug_stream);
	for( size_t i=0; i<p->fields.len; i++ ) {
		const struct {
			uint16_t offset, size;
		} *info = harbol_array_get(&p->fields, i, sizeof *info);
		fprintf(debug_stream, "index: %zu -> p's offset: '%u' | size: '%u'\n", i, info->offset, info->size);
	}
	fputs("\n", debug_stream);
	
	harbol_tuple_free(&p);
	
	p = harbol_tuple_new(4, (size_t[]){ sizeof(char), sizeof(char), sizeof(char), sizeof(char) }, false);
	assert( p );
	fprintf(debug_stream, "char p's size: '%zu'\n", p->len);
	
	fputs("\ntuple :: printing byte 4-tuple fields.\n", debug_stream);
	for( size_t i=0; i<p->fields.len; i++ ) {
		const struct {
			uint16_t offset, size;
		} *info = harbol_array_get(&p->fields, i, sizeof *info);
		fprintf(debug_stream, "index: %zu -> p's offset: '%u' | size: '%u'\n", i, info->offset, info->size);
	}
	fputs("\n", debug_stream);
	
	harbol_tuple_free(&p);
	
	p = harbol_tuple_new(4, (const size_t[]){ sizeof(int64_t), sizeof(char), sizeof(int), sizeof(short) }, false);
	assert( p );
	fprintf(debug_stream, "p's size: '%zu'\n", p->len);
	
	fputs("\ntuple :: printing 4-tuple fields.\n", debug_stream);
	for( size_t i=0; i<p->fields.len; i++ ) {
		const struct {
			uint16_t offset, size;
		} *info = harbol_array_get(&p->fields, i, sizeof *info);
		fprintf(debug_stream, "index: %zu -> p's offset: '%u' | size: '%u'\n", i, info->offset, info->size);
	}
	fputs("\n", debug_stream);
	harbol_tuple_free(&p);
	
	p = harbol_tuple_new(3, (size_t[]){ sizeof(int), sizeof(char), sizeof(short) }, false);
	assert( p );
	fprintf(debug_stream, "p's size: '%zu'\n", p->len);
	
	fputs("\ntuple :: printing newly aligned 3-tuple fields.\n", debug_stream);
	for( size_t i=0; i<p->fields.len; i++ ) {
		const struct {
			uint16_t offset, size;
		} *info = harbol_array_get(&p->fields, i, sizeof *info);
		fprintf(debug_stream, "index: %zu -> p's offset: '%u' | size: '%u'\n", i, info->offset, info->size);
	}
	fputs("\n", debug_stream);
	
	/// free tuple
	harbol_tuple_clear(p);
	fprintf(debug_stream, "p's item is null? '%s'\n", p->datum != NULL ? "no" : "yes");
	harbol_tuple_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n\n", p != NULL ? "no" : "yes");
}