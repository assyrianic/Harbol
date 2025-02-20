#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "msg_sys.h"

void test_harbol_msg_sys(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

int main(void) {
	FILE *debug_stream = fopen("harbol_msg_sys_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_msg_sys(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_msg_sys(FILE *const debug_stream) {
	( void )debug_stream;
	size_t errc = 0, warnc = 0;
	harbol_emit_msg_to_stream(&errc, stdout, "test err file 1", "big err", COLOR_RED, &( uint32_t ){1}, &( uint32_t ){0}, "big cheesery %f", 1.);
	harbol_emit_msg_to_stream(&errc, stdout, "test err file 2", "baby err", COLOR_RED, nullptr, nullptr, "big cheesery %f", 2.);
	harbol_emit_msg_to_stream(&errc, stdout, "test err file 3", "middle err", COLOR_RED, &( uint32_t ){1}, nullptr, "big cheesery %f", 3.);
	harbol_emit_msg_to_stream(&errc, stdout, "test err file 4", "just err", COLOR_RED, &( uint32_t ){1}, nullptr, "big cheesery %zu", errc);
	
	harbol_emit_msg_to_stream(&warnc, stdout, "test warn file 1", "big warner", COLOR_MAGENTA, &( uint32_t ){1}, &( uint32_t ){0}, "biggen cheesery %f", 1.);
	harbol_emit_msg_to_stream(&warnc, stdout, "test warn file 2", "baby warner", COLOR_MAGENTA, nullptr, nullptr, "biggen cheesery %f", 2.);
	harbol_emit_msg_to_stream(&warnc, stdout, "test warn file 3", "middle warner", COLOR_MAGENTA, &( uint32_t ){1}, nullptr, "biggen cheesery %f", 3.);
	harbol_emit_msg_to_stream(&warnc, stdout, "test warn file 4", "just warner", COLOR_MAGENTA, &( uint32_t ){1}, nullptr, "biggen cheesery %zu", warnc);
}