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

int main(void) {
	FILE *debug_stream = fopen("harbol_html_gen_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_html_gen(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_html_gen(FILE *const debug_stream) {
	/// Test allocation and initializations
	fputs("html gen :: test allocation/initialization.\n", debug_stream);
	
	/// free data
	fputs("\nhtml gen :: test destruction.\n", debug_stream);
	
	fprintf(debug_stream, "i's table is null?\n");
}
