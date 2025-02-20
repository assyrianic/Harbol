#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "msg_span.h"

void test_harbol_msg_span(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

int main(void) {
	FILE *debug_stream = fopen("harbol_msg_span_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_msg_span(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_msg_span(FILE *const debug_stream) {
	( void )debug_stream;
	bool res = false;
	
	fputs("msg span :: test allocation / initialization.\n", debug_stream);
	//char const *a = "for( size_t i=0; i < N; i++ ) {\n\tif( i==9 ) {\n\t\tprint(\"sexy\")\n\t}\n}";
	char const *filename = "msg_span.c";
	
	struct HarbolMsgSpan msg_span = harbol_msg_span_make(filename, true, &res);
	
	fputs("\nmsg span :: test line creation.\n", debug_stream);
	for( size_t i=0; i < msg_span.src.len; i++ ) {
		uint32_t const start = harbol_msg_span_line_start_offs(&msg_span, i);
		size_t const len = harbol_msg_span_get_line_len(&msg_span, i);
		fprintf(debug_stream, "line %zu : '%.*s'\n", i+1, (int)(len), &msg_span.src.code.cstr[start]);
	}
	
	struct HarbolTokenSpan const test_span = { 85, 129, 4, 35 };
	harbol_msg_span_add_label(&msg_span, test_span, COLOR_MAGENTA, '^', COLOR_GREEN, "this code is bad & you should feel bad.");
	harbol_msg_span_add_label(&msg_span, test_span, COLOR_GREEN, '=',  COLOR_MAGENTA, "big lel");
	
	struct HarbolTokenSpan const test_span2 = { test_span.line_start, test_span.line_end, 7, 42 };
	harbol_msg_span_add_label(&msg_span, test_span2, COLOR_WHITE, L'☝', COLOR_CYAN, "time to rewrite in rust?");
	harbol_msg_span_add_note(&msg_span, COLOR_YELLOW, "= help :: try doing this instead.");
	
	harbol_msg_span_emit_to_stream(&msg_span, nullptr, stdout, filename, "big err", "E0001", COLOR_RED, &test_span.line_start, &test_span.colm_start, "yo, you got a big error mang. %zu", msg_span.src.len);
	
	fputs("msg span :: test deallocation / freeing.\n", debug_stream);
	harbol_msg_span_clear(&msg_span);
}