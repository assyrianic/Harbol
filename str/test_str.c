#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "str.h"

void test_harbol_string(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

int main(void)
{
	FILE *debug_stream = fopen("harbol_string_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	
	test_harbol_string(debug_stream);
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}

void test_harbol_string(FILE *const debug_stream)
{
	if( debug_stream==NULL )
		return;
	
	/// Test allocation and initializations
	fputs("string :: test allocation/initialization.", debug_stream);
	fputs("\n", debug_stream);
	struct HarbolString *p = harbol_string_new("test ptr with cstr!");
	assert( p );
	fputs(p->cstr, debug_stream);
	fprintf(debug_stream, "\np's string len '%zu' | strlen val '%zu'\n", p->len, strlen(p->cstr));
	fputs("\n", debug_stream);
	
	struct HarbolString i = harbol_string_make("test stk with cstr!", &( bool ){false});
	fputs(i.cstr, debug_stream);
	fprintf(debug_stream, "\ni's string len '%zu' | strlen val '%zu'\n", i.len, strlen(i.cstr));
	fputs("\n", debug_stream);
	
	/// test appending individual chars.
	fputs("string :: test appending individual chars.", debug_stream);
	fputs("\n", debug_stream);
	/// correct output: test ptr with cstr!6
	harbol_string_add_char(p, ' ');
	harbol_string_add_char(p, '6');
	fputs(p->cstr, debug_stream);
	fputs("\n", debug_stream);
	
	harbol_string_add_char(&i, ' ');
	harbol_string_add_char(&i, '6');
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// test appending strings.
	fputs("string :: test appending C strings.", debug_stream);
	fputs("\n", debug_stream);
	harbol_string_add_cstr(p, " \'new string!\'");
	fputs(p->cstr, debug_stream);
	fputs("\n", debug_stream);
	
	harbol_string_add_cstr(&i, " \'new string!\'");
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// test appending string objects.
	fputs("\nstring :: test appending string objects.\n", debug_stream);
	harbol_string_copy_cstr(p, "A");
	harbol_string_copy_cstr(&i, "B");
	harbol_string_add_str(p, &i);
	harbol_string_add_str(&i, p);
	
	/// correct output: AB
	fputs(p->cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// correct output: BAB
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// test copying string objects.
	fputs("\nstring :: test copying string objects.\n", debug_stream);
	harbol_string_copy_cstr(p, "copied from ptr!");
	harbol_string_add_str(&i, p);
	fputs(p->cstr, debug_stream);
	fputs("\n", debug_stream);
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// test string formatting.
	fputs("\nstring :: test string formatting.\n", debug_stream);
	harbol_string_clear(&i);
	//harbol_string_reserve(&i, 100);
	harbol_string_format(&i, true, "%i + %f + %i", 900, 4242.2, 10);
	fputs(i.cstr, debug_stream);
	fprintf(debug_stream, "\ni's string len '%zu' | strlen val '%zu'\n", i.len, strlen(i.cstr));
	fputs("\n", debug_stream);
	harbol_string_format(&i, true, "%i + %f", 900, 4242.2);
	fputs(i.cstr, debug_stream);
	fprintf(debug_stream, "\ni's string len '%zu' | strlen val '%zu'\n", i.len, strlen(i.cstr));
	fputs("\n", debug_stream);
	
	/// test string concatenation formatting.
	fputs("\nstring :: test string concatenation formatting.\n", debug_stream);
	harbol_string_clear(&i);
	harbol_string_format(&i, true, "%i + %f + %i + ", 900, 4242.2, 10);
	fputs(i.cstr, debug_stream);
	fprintf(debug_stream, "\ni's string len '%zu' | strlen val '%zu'\n", i.len, strlen(i.cstr));
	
	harbol_string_format(&i, false, "%i + %f + %i", 900, 4242.2, 10);
	fputs(i.cstr, debug_stream);
	fprintf(debug_stream, "\ni's string len '%zu' | strlen val '%zu'\n", i.len, strlen(i.cstr));
	fputs("\n", debug_stream);
	
	/// test reversing string.
	fputs("\nstring :: test reversing string.\n", debug_stream);
	harbol_string_clear(&i);
	harbol_string_clear(p);
	i = harbol_string_make("test", &( bool ){false});
	harbol_string_reverse(&i);
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	harbol_string_clear(&i);
	i = harbol_string_make("abcd", &( bool ){false});
	harbol_string_reverse(&i);
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	harbol_string_clear(&i);
	i = harbol_string_make("hello world!", &( bool ){false});
	harbol_string_reverse(&i);
	fputs(i.cstr, debug_stream);
	fputs("\n", debug_stream);
	
	/// test string switch
	fputs("\nstring :: test removing chars.\n", debug_stream);
	
	const size_t removed = harbol_string_rm_char(&i, 'l');
	fprintf(debug_stream, "i's string: '%s', l's removed: %zu\n", i.cstr, removed);
	
	/// free data
	fputs("string :: test destruction.", debug_stream);
	fputs("\n", debug_stream);
	harbol_string_clear(&i);
	fprintf(debug_stream, "i's string is null? '%s'\n", i.cstr != NULL ? "no" : "yes");
	
	harbol_string_clear(p);
	fprintf(debug_stream, "p's string is null? '%s'\n", p->cstr != NULL ? "no" : "yes");
	harbol_string_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}
