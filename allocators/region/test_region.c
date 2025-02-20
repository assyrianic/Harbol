#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "region.h"

void test_harbol_region(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void) {
	FILE *debug_stream = fopen("harbol_region_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_region(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_region(FILE *const debug_stream) {
	/// Test allocation and initializations
	fputs("region :: test allocation/initialization.\n", debug_stream);
	
	struct HarbolRegion i = harbol_region_make(nullptr, sizeof(union Value) * 10, LVAL_PTR(bool, false));
	fprintf(debug_stream, "remaining region mem: '%zu'\n", harbol_region_remaining(&i));
	
	fputs("\nregion :: test allocing values.\n", debug_stream);
	union Value *valtable[10] = {nullptr};
	for( size_t n=0; n < (1[&valtable] - valtable); n++ ) {
		valtable[n] = harbol_region_alloc(&i, sizeof *valtable[0]);
		valtable[n]->int64 = n + 1;
		fprintf(debug_stream, "valtable[%zu]: %" PRIi64 "\nremaining region mem: '%zu'\n", n, valtable[n]->int64, harbol_region_remaining(&i));
	}
	
	fputs("\nregion :: test allocing differently sized values.\n", debug_stream);
	harbol_region_clear(&i);
	i = harbol_region_make(nullptr, 1000, LVAL_PTR(bool, false));
	
	/// on creation, the offset ptr for the region points to invalid memory.
	fprintf(debug_stream, "pre-alloc offset: '%zu'\n", i.offs);
	
	float32_t *f = harbol_region_alloc(&i, sizeof *f);
	*f = 32.f;
	fprintf(debug_stream, "post-alloc offset: '%zu'\n", i.offs);
	
	
	float64_t (*v)[3] = harbol_region_alloc(&i, sizeof *v);
	(*v)[0] = 3.;
	(*v)[1] = 5.;
	(*v)[2] = 10.;
	fprintf(debug_stream, "remaining region mem: '%zu'\nf value: %" PRIf32 "\nvec values: { %" PRIf64 ", %" PRIf64 ", %" PRIf64 " } | is aligned? %u\n", harbol_region_remaining(&i), *f, (*v)[0], (*v)[1], (*v)[2], is_ptr_aligned(v, sizeof(uintptr_t)));
	
	/// free data
	fputs("\nregion :: test destruction.\n", debug_stream);
	harbol_region_clear(&i);
	fprintf(debug_stream, "i's base is null? '%s'\n", i.mem != nullptr? "no" : "yes");
}