#ifndef HARBOL_TREE_INCLUDED
#	define HARBOL_TREE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../array/array.h"

struct HarbolTree {
	struct HarbolArray kids; /// []*HarbolTree
	size_t             len;
	uint8_t           *data;
};

HARBOL_EXPORT NO_NULL struct HarbolTree *harbol_tree_new(void const *val, size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_tree_init(struct HarbolTree *tree, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL void harbol_tree_clear(struct HarbolTree *tree);
HARBOL_EXPORT NO_NULL void harbol_tree_free(struct HarbolTree **treeref);

HARBOL_EXPORT NO_NULL void *harbol_tree_get(struct HarbolTree const *tree);
HARBOL_EXPORT NO_NULL size_t harbol_tree_len(struct HarbolTree const *tree);
HARBOL_EXPORT NO_NULL bool harbol_tree_set(struct HarbolTree *tree, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_tree_insert_val(struct HarbolTree *tree, void const *val, size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_tree_insert_node(struct HarbolTree *tree, struct HarbolTree **child_ref);

HARBOL_EXPORT NO_NULL bool harbol_tree_rm_node(struct HarbolTree *tree, struct HarbolTree **child_ref);
HARBOL_EXPORT NO_NULL bool harbol_tree_rm_index(struct HarbolTree *tree, size_t index);
HARBOL_EXPORT NO_NULL bool harbol_tree_rm_val(struct HarbolTree *tree, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL struct HarbolTree *harbol_tree_get_node_by_index(struct HarbolTree const *tree, size_t index);
HARBOL_EXPORT NO_NULL struct HarbolTree *harbol_tree_get_node_by_val(struct HarbolTree const *tree, void const *val, size_t datasize);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_TREE_INCLUDED */
