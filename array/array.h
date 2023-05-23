#ifndef HARBOL_ARRAY_INCLUDED
#	define HARBOL_ARRAY_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


enum { ARRAY_DEFAULT_SIZE = 4 };

struct HarbolArray {
	uint8_t *table;
	size_t   cap, len;
};


HARBOL_EXPORT NO_NULL bool harbol_array_init(struct HarbolArray *vec, size_t datasize, size_t init_size);
HARBOL_EXPORT NO_NULL struct HarbolArray harbol_array_make(size_t datasize, size_t init_size, bool *res);
HARBOL_EXPORT NO_NULL struct HarbolArray harbol_array_make_from_array(void *buf, size_t cap, size_t len);

/// creator funcs.
HARBOL_EXPORT struct HarbolArray *harbol_array_new(size_t const datasize, size_t const init_size);
HARBOL_EXPORT NO_NULL struct HarbolArray *harbol_array_new_from_array(void *const buf, size_t const cap, size_t const len);


/// clean up funcs.
HARBOL_EXPORT NO_NULL void harbol_array_clear(struct HarbolArray *const vec);
HARBOL_EXPORT NO_NULL void harbol_array_free(struct HarbolArray **const vecref);
HARBOL_EXPORT NO_NULL void harbol_array_cleanup(struct HarbolArray **const vecref);


/// array info getters.
HARBOL_EXPORT NO_NULL void *const *harbol_array_data(struct HarbolArray const *const vec);
HARBOL_EXPORT NO_NULL size_t harbol_array_cap(struct HarbolArray const *const vec);
HARBOL_EXPORT NO_NULL size_t harbol_array_len(struct HarbolArray const *const vec);


/// array table ops.
HARBOL_EXPORT NO_NULL bool harbol_array_grow(struct HarbolArray *const vec, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_resize(struct HarbolArray *const vec, size_t const datasize, size_t const new_cap);
HARBOL_EXPORT NO_NULL bool harbol_array_shrink(struct HarbolArray *const vec, size_t const datasize, bool const exact_fit);
HARBOL_EXPORT NO_NULL void harbol_array_wipe(struct HarbolArray *const vec, size_t const datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_empty(struct HarbolArray const *const vec);
HARBOL_EXPORT NO_NULL bool harbol_array_full(struct HarbolArray const *const vec);


/// array to array ops.
HARBOL_EXPORT NO_NULL bool harbol_array_add(struct HarbolArray *const vecA, struct HarbolArray const *const vecB, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_copy(struct HarbolArray *const vecA, struct HarbolArray const *const vecB, size_t const datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_len_diff(struct HarbolArray const *const vecA, struct HarbolArray const *const vecB);
HARBOL_EXPORT NO_NULL size_t harbol_array_cap_diff(struct HarbolArray const *const vecA, struct HarbolArray const *const vecB);


/// array data ops.
HARBOL_EXPORT NO_NULL bool harbol_array_insert(struct HarbolArray *const vec, void const *const val, size_t const datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_append(struct HarbolArray *const vec, void const *const val, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_fill(struct HarbolArray *const vec, void const *const val, size_t const datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_pop(struct HarbolArray *const vec, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_pop_ex(struct HarbolArray *const vec, void *const val, size_t const datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_peek(struct HarbolArray const *const vec, size_t const datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_peek_ex(struct HarbolArray const *const vec, void *const val, size_t const datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_get(struct HarbolArray const *const vec, size_t const index, size_t const datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_get_ex(struct HarbolArray const *const vec, size_t const index, void *const val, size_t const datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_set(struct HarbolArray *const vec, size_t const index, void const *const val, size_t const datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_swap(struct HarbolArray *const vec, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_shift_up(struct HarbolArray *const vec, size_t const index, size_t const datasize, size_t const amount);

HARBOL_EXPORT NO_NULL size_t harbol_array_item_count(struct HarbolArray const *const vec, void const *const val, size_t const datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_index_of(struct HarbolArray const *const vec, void const *const val, size_t const datasize, size_t const starting_index);


/// deletion funcs.
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_index(struct HarbolArray *const vec, size_t const index, size_t const datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_range(struct HarbolArray *const vec, size_t const index, size_t const datasize, size_t const range);
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_val(struct HarbolArray *const vec, void const *const val, size_t const datasize);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_ARRAY_INCLUDED */
