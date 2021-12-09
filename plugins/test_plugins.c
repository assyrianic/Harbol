#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "plugins.h"

void test_harbol_plugins(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_plugins_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_plugins(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}



static NO_NULL void on_plugin_load(struct HarbolPlugin *const restrict pl_ctxt, void *const restrict userdata, const bool reloading) {
	FILE *const restrict debug_stream = userdata;
	fprintf(debug_stream, "\nplugins :: on plugin load! - path :: %s\n", pl_ctxt->path);
}
static NO_NULL void on_plugin_unload(struct HarbolPlugin *const restrict pl_ctxt, void *const restrict userdata, const bool reloading) {
	FILE *const restrict debug_stream = userdata;
	fprintf(debug_stream, "\nplugins :: on plugin unload! - path :: %s\n", pl_ctxt->path);
}

void test_harbol_plugins(FILE *const debug_stream)
{
	fputs("plugins :: test init.\n", debug_stream);
	
	struct HarbolPlugin pl1 = {0}; harbol_plugin_load(&pl1, "./test_harbol_plugins/test_plugin" LIB_EXT, on_plugin_load, debug_stream);
	struct HarbolPlugin pl2 = {0}; harbol_plugin_load(&pl2, "./test_harbol_plugins/plugin_subfolder/plugin" LIB_EXT, on_plugin_load, debug_stream);
	
	harbol_plugin_clear(&pl1, on_plugin_unload, debug_stream);
	harbol_plugin_clear(&pl2, on_plugin_unload, debug_stream);
	fprintf(debug_stream, "\nplugins :: \n");
}
