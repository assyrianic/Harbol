#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "bistack.h"

void test_harbol_bistack(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void) {
	FILE *debug_stream = fopen("harbol_bistack_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_bistack(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_bistack(FILE *const debug_stream) {
	/// Test allocation and initializations
	fputs("bistack :: test allocation/initialization.\n", debug_stream);
	
	struct HarbolBiStack i = harbol_bistack_make(100, &( bool ){false});
	fprintf(debug_stream, "remaining bistack mem: '%zu'\n", harbol_bistack_get_margins(&i));
	
	/// test giving memory
	fputs("bistack :: test giving memory.\n", debug_stream);
	fputs("\nbistack :: allocating int ptr-array from back.\n", debug_stream);
	int *p = harbol_bistack_alloc_back(&i, sizeof *p * 20);
	fprintf(debug_stream, "p is null? '%s'\n", p? "no" : "yes");
	if( p ) {
		p[0] = 500;
		fprintf(debug_stream, "p's value: %i\n", *p);
	}
	fprintf(debug_stream, "remaining bistack mem: '%zu'\n", harbol_bistack_get_margins(&i));
	
	fputs("\nbistack :: allocating f32 ptr-array from front.\n", debug_stream);
	float32_t *f = harbol_bistack_alloc_front(&i, sizeof *f * 4);
	fprintf(debug_stream, "f is null? '%s'\n", f? "no" : "yes");
	if( f ) {
		f[0] = 500.f;
		fprintf(debug_stream, "f's value: %f\n", *f);
	}
	fprintf(debug_stream, "remaining bistack mem: '%zu'\n", harbol_bistack_get_margins(&i));
	
	fputs("bistack :: test reseting stack memory.\n", debug_stream);
	harbol_bistack_reset_all(&i);
	
	fputs("\nbistack :: allocating int ptr-array from front.\n", debug_stream);
	p = harbol_bistack_alloc_front(&i, sizeof *p * 20);
	fprintf(debug_stream, "p is null? '%s'\n", p? "no" : "yes");
	if( p ) {
		p[0] = 500;
		fprintf(debug_stream, "p's value: %i\n", *p);
	}
	fprintf(debug_stream, "remaining bistack mem: '%zu'\n", harbol_bistack_get_margins(&i));
	
	fputs("\nbistack :: allocating f32 ptr-array from back.\n", debug_stream);
	f = harbol_bistack_alloc_back(&i, sizeof *f * 4);
	fprintf(debug_stream, "f is null? '%s'\n", f? "no" : "yes");
	if( f ) {
		f[0] = 500.f;
		fprintf(debug_stream, "f's value: %f\n", *f);
	}
	fprintf(debug_stream, "remaining bistack mem: '%zu'\n", harbol_bistack_get_margins(&i));
	
	
	/// free data
	fputs("\nbistack :: test destruction.\n", debug_stream);
	harbol_bistack_clear(&i);
	fprintf(debug_stream, "i's heap is null? '%s'\n", i.mem != nullptr? "no" : "yes");
}