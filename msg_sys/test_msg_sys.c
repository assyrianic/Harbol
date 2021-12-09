#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "msg_sys.h"

void test_harbol_msg_sys(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

int main(void)
{
	FILE *debug_stream = fopen("harbol_msg_sys_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_msg_sys(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_msg_sys(FILE *const debug_stream)
{
	( void )debug_stream;
	size_t errc = 0, warnc = 0;
	harbol_err_msg(&errc, "test err file 1", "big err", &( size_t ){1}, &( size_t ){0}, "big cheesery %f", 1.);
	harbol_err_msg(&errc, "test err file 2", "baby err", NULL, NULL, "big cheesery %f", 2.);
	harbol_err_msg(&errc, "test err file 3", "middle err", &( size_t ){1}, NULL, "big cheesery %f", 3.);
	harbol_err_msg(&errc, "test err file 4", "just err", &( size_t ){1}, NULL, "big cheesery %zu", errc);

	harbol_warn_msg(&warnc, "test warn file 1", "big warner", &( size_t ){1}, &( size_t ){0}, "biggen cheesery %f", 1.);
	harbol_warn_msg(&warnc, "test warn file 2", "baby warner", NULL, NULL, "biggen cheesery %f", 2.);
	harbol_warn_msg(&warnc, "test warn file 2", "middle warner", &( size_t ){1}, NULL, "biggen cheesery %f", 3.);
	harbol_warn_msg(&warnc, "test warn file 3", "just warner", &( size_t ){1}, NULL, "biggen cheesery %zu", warnc);
}
