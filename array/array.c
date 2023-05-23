#include "array.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif



static NO_NULL bool harbol_array_resizer(struct HarbolArray *const restrict vec, size_t const new_size, size_t const element_size) {
	if( vec->cap==new_size ) {
		return true;
	}
	
	uint8_t *const new_table = harbol_recalloc(vec->table, new_size, element_size, vec->cap);
	if( new_table==NULL ) {
		return false;
	}
	vec->table = new_table;
	vec->cap   = new_size;
	return true;
}


HARBOL_EXPORT bool harbol_array_init(struct HarbolArray *const vec, size_t const datasize, size_t const init_size) {
	harbol_array_resizer(vec, (init_size < ARRAY_DEFAULT_SIZE? ARRAY_DEFAULT_SIZE : init_size), datasize);
	return vec->table != NULL;
}

HARBOL_EXPORT struct HarbolArray harbol_array_make(size_t const datasize, size_t const init_size, bool *const res) {
	struct HarbolArray array = {0};
	*res = harbol_array_init(&array, datasize, init_size);
	return array;
}

HARBOL_EXPORT struct HarbolArray harbol_array_make_from_array(void *const buf, size_t const cap, size_t const len) {
	return( struct HarbolArray ){ .table = buf, .cap = cap, .len = len };
}

/// creator funcs.
HARBOL_EXPORT struct HarbolArray *harbol_array_new(size_t const datasize, size_t const init_size) {
	struct HarbolArray *vec = calloc(1, sizeof *vec);
	if( vec==NULL || !harbol_array_init(vec, datasize, init_size) ) {
		free(vec); vec = NULL;
	}
	return vec;
}

HARBOL_EXPORT struct HarbolArray *harbol_array_new_from_array(void *const buf, size_t const cap, size_t const len) {
	struct HarbolArray *const restrict vec = calloc(1, sizeof *vec);
	if( vec != NULL ) {
		*vec = harbol_array_make_from_array(buf, cap, len);
	}
	return vec;
}


/// clean up funcs.
HARBOL_EXPORT void harbol_array_clear(struct HarbolArray *const vec) {
	free(vec->table); vec->table = NULL;
	vec->cap = vec->len = 0;
}
HARBOL_EXPORT void harbol_array_free(struct HarbolArray **const vecref) {
	free(*vecref); *vecref = NULL;
}
HARBOL_EXPORT void harbol_array_cleanup(struct HarbolArray **const vecref) {
	if( *vecref != NULL ) {
		harbol_array_clear(*vecref);
	}
	harbol_array_free(vecref);
}


/// array info getters.
HARBOL_EXPORT void *const *harbol_array_data(struct HarbolArray const *const vec) {
	return ( void *const * )(&vec->table);
}
HARBOL_EXPORT size_t harbol_array_cap(struct HarbolArray const *const vec) {
	return vec->cap;
}
HARBOL_EXPORT size_t harbol_array_len(struct HarbolArray const *const vec) {
	return vec->len;
}


/// array table ops.
HARBOL_EXPORT bool harbol_array_grow(struct HarbolArray *const vec, size_t const datasize) {
	size_t const old_cap = vec->cap;
	harbol_array_resizer(vec, (vec->cap==0? ARRAY_DEFAULT_SIZE : next_pow_of_2(vec->cap << 1)), datasize);
	return vec->cap > old_cap;
}
HARBOL_EXPORT bool harbol_array_resize(struct HarbolArray *const vec, size_t const datasize, size_t const new_cap) {
	size_t const old_cap = vec->cap;
	harbol_array_resizer(vec, (vec->cap==0 || new_cap==0? ARRAY_DEFAULT_SIZE : next_pow_of_2(new_cap)), datasize);
	return vec->cap != old_cap;
}
HARBOL_EXPORT bool harbol_array_shrink(struct HarbolArray *const vec, size_t const datasize, bool const exact_fit) {
	if( vec->cap <= ARRAY_DEFAULT_SIZE || vec->len==0 ) {
		return false;
	}
	
	size_t const old_cap = vec->cap;
	harbol_array_resizer(vec, (exact_fit? vec->len : next_pow_of_2(vec->len)), datasize);
	return old_cap > vec->cap;
}

HARBOL_EXPORT void harbol_array_wipe(struct HarbolArray *const vec, size_t const datasize) {
	if( vec->table==NULL ) {
		return;
	}
	vec->len = 0;
	memset(&vec->table[0], 0, vec->cap * datasize);
}

HARBOL_EXPORT bool harbol_array_empty(struct HarbolArray const *const vec) {
	return( vec->table==NULL || vec->cap==0 || vec->len==0 );
}
HARBOL_EXPORT bool harbol_array_full(struct HarbolArray const *const vec) {
	return( vec->len >= vec->cap );
}


/// array to array ops.
HARBOL_EXPORT bool harbol_array_add(struct HarbolArray *const vecA, struct HarbolArray const *const vecB, size_t const datasize) {
	if( vecA->table==NULL || vecB->table==NULL || (vecA->len + vecB->len) >= vecA->cap ) {
		return false;
	}
	memcpy(&vecA->table[vecA->len * datasize], vecB->table, vecB->len * datasize);
	vecA->len += vecB->len;
	return true;
}
HARBOL_EXPORT bool harbol_array_copy(struct HarbolArray *const vecA, struct HarbolArray const *const vecB, size_t const datasize) {
	if( vecA==vecB ) {
		return true;
	} else if( vecB->table==NULL || vecA->table==NULL ) {
		return false;
	}
	harbol_array_wipe(vecA, datasize);
	vecA->len = (vecA->cap < vecB->len? vecA->cap : vecB->len);
	memcpy(vecA->table, vecB->table, vecA->len * datasize);
	return true;
}
HARBOL_EXPORT size_t harbol_array_len_diff(struct HarbolArray const *const vecA, struct HarbolArray const *const vecB) {
	return( vecA->len > vecB->len )? (vecA->len - vecB->len) : (vecB->len - vecA->len);
}
HARBOL_EXPORT size_t harbol_array_cap_diff(struct HarbolArray const *const vecA, struct HarbolArray const *const vecB) {
	return( vecA->cap > vecB->cap )? (vecA->cap - vecB->cap) : (vecB->cap - vecA->cap);
}


/// array data ops.
HARBOL_EXPORT bool harbol_array_insert(struct HarbolArray *const vec, void const *const val, size_t const datasize) {
	if( vec->table==NULL || vec->len >= vec->cap ) {
		return false;
	}
	memcpy(&vec->table[vec->len++ * datasize], val, datasize);
	return true;
}
HARBOL_EXPORT size_t harbol_array_append(struct HarbolArray *const vec, void const *const val, size_t const datasize) {
	if( vec->table==NULL || vec->len >= vec->cap ) {
		return SIZE_MAX;
	}
	
	size_t const index = vec->len;
	vec->len++;
	memcpy(&vec->table[index * datasize], val, datasize);
	return index;
}
HARBOL_EXPORT bool harbol_array_fill(struct HarbolArray *const vec, void const *const val, size_t const datasize) {
	if( vec->table==NULL ) {
		return false;
	}
	for( size_t i=0; i < vec->cap; i++ ) {
		memcpy(&vec->table[i * datasize], val, datasize);
	}
	vec->len = vec->cap;
	return true;
}

HARBOL_EXPORT void *harbol_array_pop(struct HarbolArray *const vec, size_t const datasize) {
	return( vec->table==NULL || vec->len==0 )? NULL : &vec->table[--vec->len * datasize];
}
HARBOL_EXPORT bool harbol_array_pop_ex(struct HarbolArray *const vec, void *const val, size_t const datasize) {
	if( vec->table==NULL || vec->len==0 ) {
		return false;
	}
	memcpy(val, &vec->table[--vec->len * datasize], datasize);
	return true;
}

HARBOL_EXPORT void *harbol_array_peek(struct HarbolArray const *const vec, size_t const datasize) {
	return( vec->table==NULL || vec->len==0 )? NULL : &vec->table[(vec->len - 1) * datasize];
}

HARBOL_EXPORT bool harbol_array_peek_ex(struct HarbolArray const *const vec, void *const val, size_t const datasize) {
	if( vec->table==NULL || vec->len==0 ) {
		return false;
	}
	memcpy(val, &vec->table[(vec->len - 1) * datasize], datasize);
	return true;
}

HARBOL_EXPORT void *harbol_array_get(struct HarbolArray const *const vec, size_t const index, size_t const datasize) {
	return( vec->table==NULL || index >= vec->len )? NULL : &vec->table[index * datasize];
}

HARBOL_EXPORT bool harbol_array_get_ex(struct HarbolArray const *const vec, size_t const index, void *const val, size_t const datasize) {
	if( vec->table==NULL || index >= vec->len ) {
		return false;
	}
	memcpy(val, &vec->table[index * datasize], datasize);
	return true;
}

HARBOL_EXPORT bool harbol_array_set(struct HarbolArray *const vec, size_t const index, void const *const val, size_t const datasize) {
	if( vec->table==NULL || index >= vec->len ) {
		return false;
	}
	memcpy(&vec->table[index * datasize], val, datasize);
	return true;
}

HARBOL_EXPORT bool harbol_array_swap(struct HarbolArray *const vec, size_t const datasize) {
	if( vec->table==NULL ) {
		return false;
	}
	for( size_t i=0, n = vec->len-1; i < (vec->len/2); i++, n-- ) {
		for( size_t x=0; x < datasize; x++ ) {
			size_t       const i_offs = (i * datasize) + x;
			size_t       const n_offs = (n * datasize) + x;
			uint_fast8_t const data   = vec->table[n_offs];
			vec->table[n_offs] = vec->table[i_offs];
			vec->table[i_offs] = data;
		}
	}
	return true;
}

HARBOL_EXPORT bool harbol_array_shift_up(struct HarbolArray *const vec, size_t const index, size_t const datasize, size_t const amount) {
	if( vec->table==NULL ) {
		return false;
	}
	return array_shift_up(vec->table, &vec->len, index, datasize, amount);
}

HARBOL_EXPORT size_t harbol_array_item_count(struct HarbolArray const *const vec, void const *const val, size_t const datasize) {
	if( vec->table==NULL ) {
		return 0;
	}
	size_t count = 0;
	for( size_t i=0; i < vec->len; i++ ) {
		count += !memcmp(&vec->table[i * datasize], val, datasize);
	}
	return count;
}
HARBOL_EXPORT size_t harbol_array_index_of(struct HarbolArray const *const vec, void const *const val, size_t const datasize, size_t const starting_index) {
	if( vec->table==NULL ) {
		return SIZE_MAX;
	}
	
	for( size_t i=starting_index; i < vec->len; i++ ) {
		if( !memcmp(&vec->table[i * datasize], val, datasize) ) {
			return i;
		}
	}
	return SIZE_MAX;
}


/// deletion funcs.
HARBOL_EXPORT bool harbol_array_del_by_index(struct HarbolArray *const vec, size_t const index, size_t const datasize) {
	if( vec->table==NULL ) {
		return false;
	} else if( index==vec->len-1 ) {
		memset(&vec->table[--vec->len * datasize], 0, datasize);
		return true;
	}
	return harbol_array_shift_up(vec, index, datasize, 1);
}
HARBOL_EXPORT bool harbol_array_del_by_range(struct HarbolArray *const vec, size_t const index, size_t const datasize, size_t const range) {
	if( vec->table==NULL ) {
		return false;
	} else if( index==0 && (index + range >= vec->len) ) {
		harbol_array_wipe(vec, datasize);
		return true;
	} else if( index==vec->len-1 ) {
		memset(&vec->table[--vec->len * datasize], 0, datasize);
		return true;
	}
	return harbol_array_shift_up(vec, index, datasize, range);
}
HARBOL_EXPORT bool harbol_array_del_by_val(struct HarbolArray *const vec, void const *const val, size_t const datasize) {
	size_t const index = harbol_array_index_of(vec, val, datasize, 0);
	return( index != SIZE_MAX )? harbol_array_del_by_index(vec, index, datasize) : false;
}
