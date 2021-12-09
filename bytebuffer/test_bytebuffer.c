#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "bytebuffer.h"

void test_harbol_bytebuffer(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_bytebuffer_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_bytebuffer(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_bytebuffer(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("bytebuffer :: test allocation/initialization.\n", debug_stream);
	struct HarbolByteBuf *p = harbol_bytebuffer_new();
	assert( p );
	
	struct HarbolByteBuf i = harbol_bytebuffer_make();
	
	/// test adding a byte.
	fputs("\nbytebuffer :: test byte appending.\n", debug_stream);
	harbol_bytebuffer_insert_byte(p, 5);
	harbol_bytebuffer_insert_byte(&i, 6);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "i[%zu]= %u\n", n, i.table[n]);
	
	fputs("\nbytebuffer :: test uint16 appending.\n", debug_stream);
	/// test integer appending
	harbol_bytebuffer_clear(&i);
	harbol_bytebuffer_clear(p);
	uint16_t ush = 50;
	harbol_bytebuffer_insert_int16(p, ush);
	harbol_bytebuffer_insert_int16(&i, ush);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "i[%zu]= %u\n", n, i.table[n]);
	
	harbol_bytebuffer_clear(&i);
	harbol_bytebuffer_clear(p);
	
	fputs("\nbytebuffer :: test uint64 appending.\n", debug_stream);
	uint64_t ull = 0xabcdefULL;
	harbol_bytebuffer_insert_int64(p, ull);
	harbol_bytebuffer_insert_int64(&i, ull);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "i[%zu]= %u\n", n, i.table[n]);
	
	fputs("\nbytebuffer :: test string appending.\n", debug_stream);
	const char *s = "supercalifragilisticexpialidocius";
	harbol_bytebuffer_insert_cstr(p, s);
	harbol_bytebuffer_insert_cstr(&i, s);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "i[%zu]= %u\n", n, i.table[n]);
	
	
	fputs("\nbytebuffer :: test range deletion.\n", debug_stream);
	harbol_bytebuffer_clear(&i);
	harbol_bytebuffer_clear(p);
	ull = 0xABCDEF1234567890;
	harbol_bytebuffer_insert_int64(p, ull);
	harbol_bytebuffer_insert_int64(&i, ull);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "pre-deletion p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "pre-deletion i[%zu]= %u\n", n, i.table[n]);
		
	fputs("\n", debug_stream);
	harbol_bytebuffer_del(p, 0, 4);
	harbol_bytebuffer_del(&i, 0, 4);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "post-deletion p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "post-deletion i[%zu]= %u\n", n, i.table[n]);
		
		
	fputs("\nbytebuffer :: test full range deletion.\n", debug_stream);
	harbol_bytebuffer_clear(&i);
	harbol_bytebuffer_clear(p);
	ull = 0xABCDEF1234567890;
	harbol_bytebuffer_insert_int64(p, ull);
	harbol_bytebuffer_insert_int64(&i, ull);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "pre-deletion p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "pre-deletion i[%zu]= %u\n", n, i.table[n]);
		
	fputs("\n", debug_stream);
	harbol_bytebuffer_del(p, 2, 3);
	harbol_bytebuffer_del(&i, 2, 3);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "post-deletion p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "post-deletion i[%zu]= %u\n", n, i.table[n]);
	
	
	fputs("\nbytebuffer :: test buffer appending.\n", debug_stream);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "pre-appending p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "pre-appending i[%zu]= %u\n", n, i.table[n]);
		
	fputs("\n", debug_stream);
	harbol_bytebuffer_append(p, &i);
	for( size_t n=0; n<p->len; n++ )
		fprintf(debug_stream, "post-appending p[%zu]= %u\n", n, p->table[n]);
	
	fputs("\n", debug_stream);
	for( size_t n=0; n<i.len; n++ )
		fprintf(debug_stream, "post-appending i[%zu]= %u\n", n, i.table[n]);
	
	
	/// free data
	fputs("\nbytebuffer :: test destruction.\n", debug_stream);
	harbol_bytebuffer_clear(&i);
	fprintf(debug_stream, "i's table is null? '%s'\n", i.table != NULL ? "no" : "yes");
	
	harbol_bytebuffer_clear(p);
	fprintf(debug_stream, "p's table is null? '%s'\n", p->table != NULL ? "no" : "yes");
	harbol_bytebuffer_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}