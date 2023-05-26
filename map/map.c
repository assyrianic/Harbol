#include "map.h"
#include <time.h>

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolMap *harbol_map_new(size_t const init_size) {
	struct HarbolMap *map = calloc(1, sizeof *map);
	if( map==NULL || !harbol_map_init(map, init_size) ) {
		free(map); map = NULL;
	}
	return map;
}

HARBOL_EXPORT struct HarbolMap harbol_map_make(size_t const init_size, bool *const res) {
	struct HarbolMap map = {0};
	*res = harbol_map_init(&map, init_size);
	return map;
}

HARBOL_EXPORT bool harbol_map_init(struct HarbolMap *const map, size_t const init_size) {
	if( !harbol_multi_calloc(init_size, 6,
						&map->keys,     sizeof *map->keys,
						&map->keylens,  sizeof *map->keylens,
						&map->datum,    sizeof *map->datum,
						&map->datalens, sizeof *map->datalens,
						&map->hashes,   sizeof *map->hashes,
						&map->buckets,  sizeof *map->buckets)
	) {
		return false;
	}
	
	map->cap = init_size;
	map->len = 0;
	
	for( size_t i=0; i < map->cap; i++ ) {
		map->buckets[i] = SIZE_MAX;
	}
	
	srand(time(NULL));
	map->seed = ( size_t )(rand());
	return true;
}

HARBOL_EXPORT void harbol_map_clear(struct HarbolMap *const map) {
	for( size_t i=0; i < map->len; i++ ) {
		free(map->keys[i]);  map->keys[i] = NULL;
		free(map->datum[i]); map->datum[i] = NULL;
	}
	harbol_multi_cleanup(6, &map->keys, &map->keylens, &map->datum, &map->hashes, &map->buckets, &map->datalens);
	map->len = map->cap = 0;
}

HARBOL_EXPORT void harbol_map_free(struct HarbolMap **const map_ref) {
	if( *map_ref==NULL ) {
		return;
	}
	harbol_map_clear(*map_ref);
	free(*map_ref); *map_ref = NULL;
}


HARBOL_EXPORT size_t harbol_map_get_entry_index(struct HarbolMap const *const map, void const *const key, size_t const keylen) {
	uint8_t const *const restrict desired_key = key;
	size_t const hash    = array_hash(desired_key, keylen, map->seed);
	size_t const mask    = map->cap - 1;
	size_t const bkt_idx = hash & mask;
	size_t const val_idx = map->buckets[bkt_idx];
	if( val_idx==SIZE_MAX ) {
		return SIZE_MAX;
	} else if( map->keys[val_idx][0]==desired_key[0] && map->keylens[val_idx]==keylen && !memcmp(map->keys[val_idx], desired_key, keylen) ) {
		return val_idx;
	}
	
	for( size_t i = bkt_idx+1; i != bkt_idx; (i = (i+1 >= mask)? ((i+1) & mask) : i+1) ) {
		/// sometimes the `bkt_idx` is at the mask index. Account for that.
		if( i > mask ) {
			continue;
		}
		
		size_t const idx = map->buckets[i];
		if( idx==SIZE_MAX ) {
			continue;
		} else if( map->keys[idx][0]==desired_key[0] && map->keylens[idx]==keylen && !memcmp(desired_key, map->keys[idx], keylen) ) {
			return idx;
		}
	}
	return SIZE_MAX;
}

HARBOL_EXPORT bool harbol_map_has_key(struct HarbolMap const *const map, void const *const desired_key, size_t const keylen) {
	return( harbol_map_get_entry_index(map, desired_key, keylen) != SIZE_MAX );
}

static bool _harbol_map_insert_entry(struct HarbolMap *const map, size_t const n) {
	if( map->len >= map->cap ) {
		return false;
	}
	
	size_t const mask    = map->cap - 1;
	size_t       bkt_idx = map->hashes[n] & mask;
	while( map->buckets[bkt_idx] != SIZE_MAX ) {
		bkt_idx++;
		if( bkt_idx >= mask ) {
			bkt_idx &= mask;
		}
	}
	map->buckets[bkt_idx] = n;
	return true;
}

HARBOL_EXPORT bool harbol_map_rehash(struct HarbolMap *const map, size_t const new_size) {
	if( !harbol_multi_recalloc(new_size, map->cap, 6,
						&map->keys,     sizeof *map->keys,
						&map->keylens,  sizeof *map->keylens,
						&map->datum,    sizeof *map->datum,
						&map->datalens, sizeof *map->datalens,
						&map->hashes,   sizeof *map->hashes,
						&map->buckets,  sizeof *map->buckets) ) {
		return false;
	}
	
	map->cap = new_size;
	for( size_t i=0; i < map->cap; i++ ) {
		map->buckets[i] = SIZE_MAX;
	}
	
	bool res = false;
	for( size_t i=0; i < map->len; i++ ) {
		res |= _harbol_map_insert_entry(map, i);
	}
	return res;
}

HARBOL_EXPORT bool harbol_map_insert(struct HarbolMap *const restrict map, void const *const key, size_t const keylen, void const *const val, size_t const datasize) {
	if( harbol_map_has_key(map, key, keylen) || (map->len >= map->cap && !harbol_map_rehash(map, map->cap << 1)) ) {
		return false;
	}
	
	size_t const val_idx = map->len;
	map->keys[val_idx] = dup_data(key, keylen);
	if( map->keys[val_idx]==NULL ) {
		return false;
	}
	
	map->datum[val_idx] = dup_data(val, datasize);
	if( map->datum[val_idx]==NULL ) {
		free(map->keys[val_idx]); map->keys[val_idx] = NULL;
		return false;
	}
	
	map->hashes[val_idx] = array_hash(key, keylen, map->seed);
	if( !_harbol_map_insert_entry(map, val_idx) ) {
		free(map->keys[val_idx]); map->keys[val_idx] = NULL;
		free(map->datum[val_idx]); map->datum[val_idx] = NULL;
		map->hashes[val_idx] = 0;
		return false;
	}
	map->keylens[val_idx]  = keylen;
	map->datalens[val_idx] = datasize;
	map->len++;
	return true;
}


HARBOL_EXPORT void *harbol_map_key_val(struct HarbolMap const *const map, void const *const val, size_t const datasize, size_t *const restrict keylen) {
	size_t const i = harbol_map_idx_val(map, val, datasize);
	if( i==SIZE_MAX ) {
		return NULL;
	} else {
		*keylen = map->keylens[i];
		return map->keys[i];
	}
}

HARBOL_EXPORT size_t harbol_map_idx_val(struct HarbolMap const *const map, void const *const val, size_t const datasize) {
	if( map->len==0 ) {
		return SIZE_MAX;
	}
	
	for( size_t i=0; i < map->len; i++ ) {
		uint8_t const *const restrict desired_val = val;
		if( map->datum[i][0]==desired_val[0] && map->datalens[i]==datasize && !memcmp(map->datum[i], val, datasize) ) {
			return i;
		}
	}
	return SIZE_MAX;
}

HARBOL_EXPORT void *harbol_map_key_get(struct HarbolMap const *const map, void const *const key, size_t const keylen) {
	size_t const entry = harbol_map_get_entry_index(map, key, keylen);
	return harbol_map_idx_get(map, entry);
}

HARBOL_EXPORT void *harbol_map_idx_get(struct HarbolMap const *const map, size_t const index) {
	return( index >= map->len )? NULL : map->datum[index];
}


HARBOL_EXPORT bool harbol_map_key_set(struct HarbolMap *const restrict map, void const *const key, size_t const keylen, void const *const val, size_t const datasize) {
	if( !harbol_map_has_key(map, key, keylen) ) {
		return harbol_map_insert(map, key, keylen, val, datasize);
	}
	
	size_t const entry = harbol_map_get_entry_index(map, key, keylen);
	return harbol_map_idx_set(map, entry, val, datasize);
}

HARBOL_EXPORT bool harbol_map_idx_set(struct HarbolMap *const restrict map, size_t const index, void const *const val, size_t const datasize) {
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

HARBOL_EXPORT bool harbol_map_key_rm(struct HarbolMap *const restrict map, void const *const key, size_t const keylen) {
	size_t const val_idx = harbol_map_get_entry_index(map, key, keylen);
	return( val_idx==SIZE_MAX )? false : harbol_map_idx_rm(map, val_idx);
}

HARBOL_EXPORT bool harbol_map_idx_rm(struct HarbolMap *const map, size_t const n) {
	if( n >= map->len ) {
		return false;
	}
	
	size_t const mask    = map->cap - 1;
	size_t const bkt_idx = map->hashes[n] & mask;
	size_t const val_idx = map->buckets[bkt_idx];
	if( val_idx==SIZE_MAX ) {
		return false;
	}
	
	free(map->keys[n]);  map->keys[n]  = NULL;
	free(map->datum[n]); map->datum[n] = NULL;
	map->hashes[n] = map->keylens[n] = map->datalens[n] = 0;
	map->buckets[bkt_idx] = SIZE_MAX;
	
	return multi_array_shift_up(&map->len, n, 1, 5,
			map->keys,     sizeof *map->keys,
			map->datum,    sizeof *map->datum,
			map->hashes,   sizeof *map->hashes,
			map->keylens,  sizeof *map->keylens,
			map->datalens, sizeof *map->datalens
	);
}