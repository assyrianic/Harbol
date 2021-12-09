#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "../str/str.h"
#include "tree.h"

void test_harbol_tree(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_tree_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_tree(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_tree(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("tree :: test allocation/initialization.\n", debug_stream);
	const size_t u_val_size = sizeof(union Value);
	struct HarbolTree *p = harbol_tree_new(&( union Value ){.int64=1}, u_val_size);
	if( p != NULL )
		fputs("tree :: allocation/initialization of p is GOOD.\n", debug_stream);
	
	
	/// Test insertions
	fputs("\ntree :: test insertions.\n", debug_stream);
	struct HarbolTree *t = harbol_tree_new(&( union Value ){.int64=20}, u_val_size);
	fprintf(debug_stream, "insertion by node success?: '%u'\n", harbol_tree_insert_node(p, &t));
	fprintf(debug_stream, "insertion by value success?: '%u'\n", harbol_tree_insert_val(p, &( union Value ){.int64=10}, u_val_size));
	
	harbol_tree_clear(p);
	
	/// Test deletions
	fputs("\ntree :: test deletions by first adding 5 children.\n", debug_stream);
	harbol_tree_insert_val(p, &( union Value ){.int64=1}, u_val_size);
	harbol_tree_insert_val(p, &( union Value ){.int64=2}, u_val_size);
	harbol_tree_insert_val(p, &( union Value ){.int64=3}, u_val_size);
	harbol_tree_insert_val(p, &( union Value ){.int64=4}, u_val_size);
	harbol_tree_insert_val(p, &( union Value ){.int64=5}, u_val_size);
	for( size_t n=0; n<p->kids.len; n++ ) {
		struct HarbolTree *child = harbol_tree_get_node_by_index(p, n);
		fprintf(debug_stream, "child #%zu value: '%" PRIi64 "'\n", n, (( const union Value* )child->data)->int64);
	}
	fputs("\ndeleting index 1\n", debug_stream);
	harbol_tree_rm_index(p, 1);
	for( size_t n=0; n<p->kids.len; n++ ) {
		struct HarbolTree *child = harbol_tree_get_node_by_index(p, n);
		fprintf(debug_stream, "child #%zu value: '%" PRIi64 "'\n", n, (( const union Value* )child->data)->int64);
	}
	/// Test delete by node reference
	fputs("\ntree :: test deletion by node reference.\n", debug_stream);
	/// delete first child!
	fputs("\ndeleting index 0\n", debug_stream);
	{
		struct HarbolTree *zero = harbol_tree_get_node_by_index(p, 0);
		harbol_tree_rm_node(p, &zero);
	}
	for( size_t n=0; n<p->kids.len; n++ ) {
		struct HarbolTree *child = harbol_tree_get_node_by_index(p, n);
		fprintf(debug_stream, "child #%zu value: '%" PRIi64 "'\n", n, (( const union Value* )child->data)->int64);
	}
	/// Test creating something of an abstract syntax tree.
	fputs("\ntree :: test creating something of an abstract syntax tree.\n", debug_stream);
	harbol_tree_free(&p);
	const size_t hstr_size = sizeof(struct HarbolString);
	
	struct HarbolString str = harbol_string_make("program", &( bool ){false});
	p = harbol_tree_new(&str, hstr_size);
	
	str = harbol_string_make("stmt", &( bool ){false});
	harbol_tree_insert_val(p, &str, hstr_size);
	
	str = harbol_string_make("if", &( bool ){false});
	struct HarbolTree *kid = harbol_tree_get_node_by_index(p, 0);
	harbol_tree_insert_val(kid, &str, hstr_size);
	
	str = harbol_string_make("cond", &( bool ){false});
	harbol_tree_insert_val(kid, &str, hstr_size);
	
	str = harbol_string_make("stmt", &( bool ){false});
	harbol_tree_insert_val(kid, &str, hstr_size);
	
	str = harbol_string_make("else", &( bool ){false});
	harbol_tree_insert_val(kid, &str, hstr_size);
	
	fprintf(debug_stream, "p's data: '%s'\n", (( const struct HarbolString* )p->data)->cstr);
	fprintf(debug_stream, "p's child data: '%s'\n", (( const struct HarbolString* )kid->data)->cstr);
	for( size_t n=0; n<kid->kids.len; n++ ) {
		struct HarbolTree *child = harbol_tree_get_node_by_index(kid, n);
		fprintf(debug_stream, "p's child's children data: '%s'\n", (( const struct HarbolString* )child->data)->cstr);
	}
	
	for( size_t n=0; n<p->kids.len; n++ ) {
		struct HarbolTree *child = harbol_tree_get_node_by_index(p, n);
		harbol_string_clear(( struct HarbolString* )child->data);
		for( size_t o=0; o<child->kids.len; o++ ) {
			struct HarbolTree *ch = harbol_tree_get_node_by_index(child, o);
			harbol_string_clear(( struct HarbolString* )ch->data);
		}
	}
	fprintf(debug_stream, "\nfreeing string data\n");
	harbol_string_clear(( struct HarbolString* )p->data);
	harbol_tree_clear(p);
	
	/// free data
	fputs("\ntree :: test destruction.\n", debug_stream);
	harbol_tree_clear(p);
	fprintf(debug_stream, "p's children vector is null? '%s'\n", p->kids.table != NULL ? "no" : "yes");
	harbol_tree_free(&p);
	fprintf(debug_stream, "p is null? '%s'\n", p != NULL ? "no" : "yes");
}
