#ifndef HARBOL_MAP_INCLUDED
#	define HARBOL_MAP_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../array/array.h"


/// ordered hash table.
struct HarbolMap {
	struct HarbolArray *buckets;
	uint8_t           **datum, **keys;
	size_t             *hashes, *keylens, cap, len, seed;
};


HARBOL_EXPORT struct HarbolMap *harbol_map_new(size_t init_size);
HARBOL_EXPORT NO_NULL struct HarbolMap harbol_map_make(size_t init_size, bool *res);
HARBOL_EXPORT NO_NULL bool harbol_map_init(struct HarbolMap *map, size_t init_size);

HARBOL_EXPORT NO_NULL void harbol_map_clear(struct HarbolMap *map);
HARBOL_EXPORT NO_NULL void harbol_map_free(struct HarbolMap **map_ref);

HARBOL_EXPORT NO_NULL bool harbol_map_has_key(const struct HarbolMap *map, const void *key, size_t keylen);

HARBOL_EXPORT NO_NULL bool harbol_map_rehash(struct HarbolMap *map, size_t new_size);

HARBOL_EXPORT NO_NULL bool harbol_map_insert(struct HarbolMap *map, const void *key, size_t keylen, const void *val, size_t datasize);

HARBOL_EXPORT NO_NULL size_t harbol_map_get_entry_index(const struct HarbolMap *map, const void *key, size_t keylen);

HARBOL_EXPORT NO_NULL void *harbol_map_key_get(const struct HarbolMap *map, const void *key, size_t keylen);
HARBOL_EXPORT NO_NULL void *harbol_map_idx_get(const struct HarbolMap *map, size_t index);

HARBOL_EXPORT NO_NULL bool harbol_map_key_set(struct HarbolMap *map, const void *key, size_t keylen, const void *val, size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_map_idx_set(struct HarbolMap *map, size_t index, const void *val, size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_map_key_rm(struct HarbolMap *map, const void *key, size_t keylen);
HARBOL_EXPORT NO_NULL bool harbol_map_idx_rm(struct HarbolMap *map, size_t index);
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MAP_INCLUDED */
