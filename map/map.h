#ifndef HARBOL_MAP_INCLUDED
#	define HARBOL_MAP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


/// ordered hash table.
struct HarbolMap {
	uint8_t **datum, **keys;
	size_t   *buckets, *hashes, *keylens, cap, len, seed;
};


HARBOL_EXPORT struct HarbolMap *harbol_map_new(size_t init_size);
HARBOL_EXPORT NO_NULL struct HarbolMap harbol_map_make(size_t init_size, bool *res);
HARBOL_EXPORT NO_NULL bool harbol_map_init(struct HarbolMap *map, size_t init_size);

HARBOL_EXPORT NO_NULL void harbol_map_clear(struct HarbolMap *map);
HARBOL_EXPORT NO_NULL void harbol_map_free(struct HarbolMap **map_ref);

HARBOL_EXPORT NO_NULL bool harbol_map_has_key(struct HarbolMap const *map, void const *key, size_t keylen);

HARBOL_EXPORT NO_NULL bool harbol_map_rehash(struct HarbolMap *map, size_t new_size);

HARBOL_EXPORT NO_NULL bool harbol_map_insert(struct HarbolMap *map, void const *key, size_t keylen, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL size_t harbol_map_get_entry_index(struct HarbolMap const *map, void const *key, size_t keylen);

HARBOL_EXPORT NO_NULL void *harbol_map_key_val(struct HarbolMap const *map, void const *val, size_t datasize, size_t *keylen);
HARBOL_EXPORT NO_NULL size_t harbol_map_idx_val(struct HarbolMap const *map, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL void *harbol_map_key_get(struct HarbolMap const *map, void const *key, size_t keylen);
HARBOL_EXPORT NO_NULL void *harbol_map_idx_get(struct HarbolMap const *map, size_t index);

HARBOL_EXPORT NO_NULL bool harbol_map_key_set(struct HarbolMap *map, void const *key, size_t keylen, void const *val, size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_map_idx_set(struct HarbolMap *map, size_t index, void const *val, size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_map_key_rm(struct HarbolMap *map, void const *key, size_t keylen);
HARBOL_EXPORT NO_NULL bool harbol_map_idx_rm(struct HarbolMap *map, size_t index);
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MAP_INCLUDED */
