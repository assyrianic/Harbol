#include "tree.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

HARBOL_EXPORT struct HarbolTree *harbol_tree_new(const void *const val, const size_t datasize)
{
	struct HarbolTree *restrict tree = calloc(1, sizeof *tree);
	if( tree==NULL || !harbol_tree_init(tree, val, datasize) ) {
		free(tree); tree = NULL;
	}
	return tree;
}

HARBOL_EXPORT bool harbol_tree_init(struct HarbolTree *const restrict tree, const void *const val, const size_t datasize) {
	tree->data = dup_data(val, datasize);
	if( tree->data==NULL ) {
		return false;
	} else if( !harbol_array_init(&tree->kids, sizeof tree, 4) ) {
		free(tree->data); tree->data = NULL;
		return false;
	}
	tree->len = datasize;
	return true;
}

HARBOL_EXPORT void harbol_tree_clear(struct HarbolTree *const tree)
{
	free(tree->data); tree->data = NULL;
	tree->len = 0;
	union {
		uint8_t            *b;
		struct HarbolTree **t;
	} conv = {tree->kids.table};
	for( size_t i=0; i<tree->kids.len; i++ ) {
		harbol_tree_free(&conv.t[i]);
	}
	harbol_array_clear(&tree->kids);
}

HARBOL_EXPORT void harbol_tree_free(struct HarbolTree **const tree_ref)
{
	if( *tree_ref==NULL )
		return;

	harbol_tree_clear(*tree_ref);
	free(*tree_ref); *tree_ref = NULL;
}

HARBOL_EXPORT void *harbol_tree_get(const struct HarbolTree *const tree) {
	return tree->data;
}

HARBOL_EXPORT NO_NULL size_t harbol_tree_len(const struct HarbolTree *tree) {
	return tree->len;
}

HARBOL_EXPORT bool harbol_tree_set(struct HarbolTree *const restrict tree, const void *const val, const size_t datasize)
{
	if( datasize==0 )
		return false;
	
	if( tree->data==NULL ) {
		tree->data = calloc(datasize, sizeof *tree->data);
		if( tree->data==NULL )
			return false;
	}
	memcpy(tree->data, val, datasize);
	tree->len = datasize;
	return true;
}

HARBOL_EXPORT bool harbol_tree_insert_val(struct HarbolTree *const restrict tree, const void *const val, const size_t datasize)
{
	if( datasize==0 || (harbol_array_full(&tree->kids) && !harbol_array_grow(&tree->kids, sizeof tree)) )
		return false;
	
	struct HarbolTree *node = harbol_tree_new(val, datasize);
	return( node==NULL || node->data==NULL ) ? false : harbol_array_insert(&tree->kids, &node, sizeof node);
}

HARBOL_EXPORT bool harbol_tree_insert_node(struct HarbolTree *const tree, struct HarbolTree **const child_ref)
{
	return( (*child_ref)->data==NULL || (harbol_array_full(&tree->kids) && !harbol_array_grow(&tree->kids, sizeof tree)) ) ? false : harbol_array_insert(&tree->kids, child_ref, sizeof *child_ref);
}

HARBOL_EXPORT bool harbol_tree_rm_node(struct HarbolTree *const tree, struct HarbolTree **const child_ref)
{
	const size_t i = harbol_array_index_of(&tree->kids, child_ref, sizeof *child_ref, 0);
	if( i==SIZE_MAX )
		return false;
	
	harbol_tree_free(child_ref);
	return harbol_array_del_by_index(&tree->kids, i, sizeof tree);
}

HARBOL_EXPORT bool harbol_tree_rm_index(struct HarbolTree *const tree, const size_t index)
{
	struct HarbolTree **const child_ref = harbol_array_get(&tree->kids, index, sizeof *child_ref);
	if( child_ref==NULL || *child_ref==NULL )
		return false;
	
	harbol_tree_free(child_ref);
	return harbol_array_del_by_index(&tree->kids, index, sizeof *child_ref);
}

HARBOL_EXPORT bool harbol_tree_rm_val(struct HarbolTree *const restrict tree, const void *const val, const size_t datasize)
{
	if( datasize==0 )
		return false;
	
	for( size_t i=0; i<tree->kids.len; i++ ) {
		struct HarbolTree **const child_ref = harbol_array_get(&tree->kids, i, sizeof *child_ref);
		if( !memcmp((*child_ref)->data, val, (*child_ref)->len) ) {
			harbol_tree_free(child_ref);
			return harbol_array_del_by_index(&tree->kids, i, sizeof *child_ref);
		}
	}
	return false;
}

HARBOL_EXPORT struct HarbolTree *harbol_tree_get_node_by_index(const struct HarbolTree *const tree, const size_t index) {
	struct HarbolTree **const child_ref = harbol_array_get(&tree->kids, index, sizeof *child_ref);
	if( child_ref==NULL || *child_ref==NULL )
		return NULL;
	return *child_ref;
}

HARBOL_EXPORT struct HarbolTree *harbol_tree_get_node_by_val(const struct HarbolTree *const tree, const void *const val, const size_t datasize)
{
	if( datasize==0 )
		return NULL;
	
	for( size_t i=0; i<tree->kids.len; i++ ) {
		struct HarbolTree **const child_ref = harbol_array_get(&tree->kids, i, sizeof *child_ref);
		if( !memcmp((*child_ref)->data, val, (*child_ref)->len) )
			return *child_ref;
	}
	return NULL;
}
