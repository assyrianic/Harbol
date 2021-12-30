#include "map.h"
#include <time.h>

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif
static bool seeded;

HARBOL_EXPORT struct HarbolMap *harbol_map_new(const size_t init_size) {
	struct HarbolMap *map = calloc(1, sizeof *map);
	if( map==NULL || !harbol_map_init(map, init_size) ) {
		free(map); map = NULL;
	}
	return map;
}

HARBOL_EXPORT struct HarbolMap harbol_map_make(const size_t init_size, bool *const res) {
	struct HarbolMap map = {0};
	*res = harbol_map_init(&map, init_size);
	return map;
}

HARBOL_EXPORT bool harbol_map_init(struct HarbolMap *const map, const size_t init_size) {
	map->keys    = calloc(init_size, sizeof *map->keys);
	map->keylens = calloc(init_size, sizeof *map->keylens);
	map->datum   = calloc(init_size, sizeof *map->datum);
	map->hashes  = calloc(init_size, sizeof *map->hashes);
	map->buckets = calloc(init_size, sizeof *map->buckets);
	if( map->keys==NULL
			|| map->keylens==NULL || map->datum==NULL
			|| map->hashes==NULL  || map->buckets==NULL ) {
		free(map->keys);    map->keys = NULL;
		free(map->keylens); map->keylens = NULL;
		free(map->datum);   map->datum = NULL;
		free(map->hashes);  map->hashes = NULL;
		free(map->buckets); map->buckets = NULL;
		return false;
	}
	map->cap = init_size;
	map->len = 0;
	
	if( !seeded ) {
		srand(time(NULL));
		seeded = true;
	}
	map->seed = ( size_t )(rand());
	return true;
}

HARBOL_EXPORT void harbol_map_clear(struct HarbolMap *const map) {
	for( size_t i=0; i<map->len; i++ ) {
		free(map->keys[i]);  map->keys[i] = NULL;
		free(map->datum[i]); map->datum[i] = NULL;
	}
	free(map->keys);    map->keys = NULL;
	free(map->keylens); map->keylens = NULL;
	free(map->datum);   map->datum = NULL;
	free(map->hashes);  map->hashes = NULL;
	
	for( size_t i=0; i<map->cap; i++ ) {
		harbol_array_clear(&map->buckets[i]);
	}
	free(map->buckets); map->buckets = NULL;
	map->len = map->cap = 0;
}

HARBOL_EXPORT void harbol_map_free(struct HarbolMap **const map_ref) {
	if( *map_ref==NULL )
		return;
	
	harbol_map_clear(*map_ref);
	free(*map_ref); *map_ref = NULL;
}


HARBOL_EXPORT bool harbol_map_has_key(const struct HarbolMap *const map, const void *const key, const size_t keylen) {
	const size_t hash = array_hash(key, keylen, map->seed);
	const size_t index = hash & (map->cap - 1);
	const struct HarbolArray *const bucket = &map->buckets[index];
	for( size_t i=0; i<bucket->len; i++ ) {
		const size_t *const n = *( const size_t** )(harbol_array_get(bucket, i, sizeof n));
		const uintptr_t real_index = (( uintptr_t )(n) - ( uintptr_t )(map->hashes)) / sizeof *n;
		if( *n==hash && map->keylens[real_index]==keylen && !memcmp(map->keys[real_index], key, keylen) )
			return true;
	}
	return false;
}

static bool _harbol_map_resize_vecs(struct HarbolMap *const map, const size_t new_size) {
	uint8_t **const new_keys = harbol_recalloc(map->keys, new_size, sizeof *map->keys, map->cap);
	if( new_keys==NULL ) { /// NULL keys leaves 'map->keys' unchanged.
		return false;
	}
	map->keys = new_keys;
	
	size_t *const new_keylens = harbol_recalloc(map->keylens, new_size, sizeof *map->keylens, map->cap);
	if( new_keylens==NULL ) {
		return false;
	}
	map->keylens = new_keylens;
	
	uint8_t **const new_datum = harbol_recalloc(map->datum, new_size, sizeof *map->datum, map->cap);
	if( new_datum==NULL ) {
		return false;
	}
	map->datum = new_datum;
	
	size_t *const new_hashes = harbol_recalloc(map->hashes, new_size, sizeof *map->hashes, map->cap);
	if( new_hashes==NULL ) {
		return false;
	}
	map->hashes = new_hashes;
	return true;
}

static bool _harbol_map_insert_entry(struct HarbolMap *const map, const size_t n) {
	const size_t index = map->hashes[n] & (map->cap - 1);
	struct HarbolArray *const bucket = &map->buckets[index];
	
	/// this will run even if the bucket is empty
	/// as a cap of 0 with len of 0 is still technically full!
	if( harbol_array_full(bucket) && !harbol_array_grow(bucket, sizeof n) )
		return false;
	
	return harbol_array_insert(bucket, &( size_t* ){ &map->hashes[n] }, sizeof map->hashes);
}

HARBOL_EXPORT bool harbol_map_rehash(struct HarbolMap *const map, const size_t new_size) {
	const size_t old_cap = map->cap;
	struct HarbolArray *curr = map->buckets;
	map->buckets = calloc(new_size, sizeof *curr);
	if( map->buckets==NULL ) {
		map->buckets = curr;
		return false;
	} else if( !_harbol_map_resize_vecs(map, new_size) ) {
		/// if buffers fail to resize, revert and don't change cap.
		free(map->buckets);
		map->buckets = curr;
		return false;
	}
	
	map->cap = new_size;
	for( size_t i=0; i<old_cap; i++ ) {
		harbol_array_clear(&curr[i]);
	}
	free(curr); curr = NULL;
	
	bool res = false;
	for( size_t i=0; i<map->len; i++ ) {
		res |= _harbol_map_insert_entry(map, i);
	}
	return res;
}

HARBOL_EXPORT bool harbol_map_insert(struct HarbolMap *const restrict map, const void *const key, const size_t keylen, const void *const val, const size_t datasize) {
	if( harbol_map_has_key(map, key, keylen) || (map->len >= map->cap && !harbol_map_rehash(map, map->cap << 1)) )
		return false;
	
	const size_t entry_index = map->len;
	map->keys[entry_index] = dup_data(key, keylen);
	if( map->keys[entry_index]==NULL ) {
		return false;
	}
	
	map->datum[entry_index] = dup_data(val, datasize);
	if( map->datum[entry_index]==NULL ) {
		free(map->keys[entry_index]); map->keys[entry_index] = NULL;
		return false;
	}
	
	map->hashes[entry_index] = array_hash(key, keylen, map->seed);
	if( !_harbol_map_insert_entry(map, entry_index) ) {
		free(map->keys[entry_index]); map->keys[entry_index] = NULL;
		free(map->datum[entry_index]); map->datum[entry_index] = NULL;
		map->hashes[entry_index] = 0;
		return false;
	}
	map->keylens[entry_index] = keylen;
	map->len++;
	return true;
}

HARBOL_EXPORT size_t harbol_map_get_entry_index(const struct HarbolMap *const map, const void *const key, const size_t keylen) {
	if( !harbol_map_has_key(map, key, keylen) )
		return SIZE_MAX;
	
	const size_t hash = array_hash(key, keylen, map->seed);
	const size_t index = hash & (map->cap - 1);
	const struct HarbolArray *const bucket = &map->buckets[index];
	for( size_t i=0; i<bucket->len; i++ ) {
		const size_t *const n = *( const size_t** )(harbol_array_get(bucket, i, sizeof n));
		const uintptr_t real_index = (( uintptr_t )(n) - ( uintptr_t )(map->hashes)) / sizeof *n;
		if( *n==hash && map->keylens[real_index]==keylen && !memcmp(map->keys[real_index], key, keylen) )
			return real_index;
	}
	return SIZE_MAX;
}


HARBOL_EXPORT void *harbol_map_key_val(const struct HarbolMap *const map, const void *const val, const size_t datasize, size_t *const restrict keylen) {
	const size_t i = harbol_map_idx_val(map, val, datasize);
	if( i==SIZE_MAX ) {
		return NULL;
	} else {
		*keylen = map->keylens[i];
		return map->keys[i];
	}
}

HARBOL_EXPORT size_t harbol_map_idx_val(const struct HarbolMap *const map, const void *const val, const size_t datasize) {
	if( map->len==0 ) {
		return SIZE_MAX;
	}
	
	for( size_t i=0; i < map->len; i++ ) {
		if( !memcmp(map->datum[i], val, datasize) ) {
			return i;
		}
	}
	return SIZE_MAX;
}

HARBOL_EXPORT void *harbol_map_key_get(const struct HarbolMap *const map, const void *const key, const size_t keylen) {
	const size_t entry = harbol_map_get_entry_index(map, key, keylen);
	return harbol_map_idx_get(map, entry);
}

HARBOL_EXPORT void *harbol_map_idx_get(const struct HarbolMap *const map, const size_t index) {
	return( index >= map->len ) ? NULL : map->datum[index];
}


HARBOL_EXPORT bool harbol_map_key_set(struct HarbolMap *const restrict map, const void *const key, const size_t keylen, const void *const val, const size_t datasize) {
	if( !harbol_map_has_key(map, key, keylen) ) {
		return harbol_map_insert(map, key, keylen, val, datasize);
	}
	
	const size_t entry = harbol_map_get_entry_index(map, key, keylen);
	return harbol_map_idx_set(map, entry, val, datasize);
}

HARBOL_EXPORT bool harbol_map_idx_set(struct HarbolMap *const restrict map, const size_t index, const void *const val, const size_t datasize) {
	if( index >= map->len ) {
		return false;
	}
	
	uint8_t *const data = dup_data(val, datasize);
	if( data==NULL ) {
		return false;
	}
	
	free(map->datum[index]);
	map->datum[index] = data;
	return true;
}

HARBOL_EXPORT bool harbol_map_key_rm(struct HarbolMap *const restrict map, const void *const key, const size_t keylen) {
	if( !harbol_map_has_key(map, key, keylen) )
		return false;
	
	const size_t hash = array_hash(key, keylen, map->seed);
	const size_t index = hash & (map->cap - 1);
	const struct HarbolArray *const bucket = &map->buckets[index];
	for( size_t i=0; i<bucket->len; i++ ) {
		const size_t *const n = *( const size_t** )(harbol_array_get(bucket, i, sizeof n));
		const uintptr_t real_index = (( uintptr_t )(n) - ( uintptr_t )(map->hashes)) / sizeof *n;
		if( *n==hash && map->keylens[real_index]==keylen && !memcmp(map->keys[real_index], key, keylen) )
			return harbol_map_idx_rm(map, real_index);
	}
	return false;
}

HARBOL_EXPORT bool harbol_map_idx_rm(struct HarbolMap *const map, const size_t n) {
	if( n >= map->len )
		return false;
	
	const size_t index = map->hashes[n] & (map->cap - 1);
	struct HarbolArray *const bucket = &map->buckets[index];
	size_t *const p = &map->hashes[n];
	const size_t entry_idx = harbol_array_index_of(bucket, &p, sizeof p, 0);
	if( entry_idx==SIZE_MAX || !harbol_array_del_by_index(bucket, entry_idx, sizeof p) )
		return false;
	
	free(map->keys[n]);  map->keys[n]  = NULL;
	free(map->datum[n]); map->datum[n] = NULL;
	map->hashes[n] = map->keylens[n] = 0;
	
	size_t
		len1 = map->len,
		len2 = map->len,
		len3 = map->len,
		len4 = map->len
	;
	harbol_gen_array_shift_up(( uint8_t* )(map->keys),    &len1, n, sizeof *map->keys,    1);
	harbol_gen_array_shift_up(( uint8_t* )(map->datum),   &len2, n, sizeof *map->datum,   1);
	harbol_gen_array_shift_up(( uint8_t* )(map->hashes),  &len3, n, sizeof *map->hashes,  1);
	harbol_gen_array_shift_up(( uint8_t* )(map->keylens), &len4, n, sizeof *map->keylens, 1);
	map->len--;
	return true;
}
