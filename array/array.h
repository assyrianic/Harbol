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
HARBOL_EXPORT struct HarbolArray *harbol_array_new(const size_t datasize, const size_t init_size);
HARBOL_EXPORT NO_NULL struct HarbolArray *harbol_array_new_from_array(void *const buf, const size_t cap, const size_t len);


/// clean up funcs.
HARBOL_EXPORT NO_NULL void harbol_array_clear(struct HarbolArray *const vec);
HARBOL_EXPORT NO_NULL void harbol_array_free(struct HarbolArray **const vecref);
HARBOL_EXPORT NO_NULL void harbol_array_cleanup(struct HarbolArray **const vecref);


/// array info getters.
HARBOL_EXPORT NO_NULL void *const *harbol_array_data(const struct HarbolArray *const vec);
HARBOL_EXPORT NO_NULL size_t harbol_array_cap(const struct HarbolArray *const vec);
HARBOL_EXPORT NO_NULL size_t harbol_array_len(const struct HarbolArray *const vec);


/// array table ops.
HARBOL_EXPORT NO_NULL bool harbol_array_grow(struct HarbolArray *const vec, const size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_resize(struct HarbolArray *const vec, const size_t datasize, const size_t new_cap);
HARBOL_EXPORT NO_NULL bool harbol_array_shrink(struct HarbolArray *const vec, const size_t datasize, const bool exact_fit);
HARBOL_EXPORT NO_NULL void harbol_array_wipe(struct HarbolArray *const vec, const size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_empty(const struct HarbolArray *const vec);
HARBOL_EXPORT NO_NULL bool harbol_array_full(const struct HarbolArray *const vec);


/// array to array ops.
HARBOL_EXPORT NO_NULL bool harbol_array_add(struct HarbolArray *const vecA, const struct HarbolArray *const vecB, const size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_copy(struct HarbolArray *const vecA, const struct HarbolArray *const vecB, const size_t datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_len_diff(const struct HarbolArray *const vecA, const struct HarbolArray *const vecB);
HARBOL_EXPORT NO_NULL size_t harbol_array_cap_diff(const struct HarbolArray *const vecA, const struct HarbolArray *const vecB);


/// array data ops.
HARBOL_EXPORT NO_NULL bool harbol_array_insert(struct HarbolArray *const vec, const void *const val, const size_t datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_append(struct HarbolArray *const vec, const void *const val, const size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_fill(struct HarbolArray *const vec, const void *const val, const size_t datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_pop(struct HarbolArray *const vec, const size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_pop_ex(struct HarbolArray *const vec, void *const val, const size_t datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_peek(const struct HarbolArray *const vec, const size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_peek_ex(const struct HarbolArray *const vec, void *const val, const size_t datasize);

HARBOL_EXPORT NO_NULL void *harbol_array_get(const struct HarbolArray *const vec, const size_t index, const size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_get_ex(const struct HarbolArray *const vec, const size_t index, void *const val, const size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_set(struct HarbolArray *const vec, const size_t index, const void *const val, const size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_array_swap(struct HarbolArray *const vec, const size_t datasize);

HARBOL_EXPORT bool harbol_gen_array_shift_up(uint8_t *buf, size_t *len, size_t index, size_t datasize, size_t amount);
HARBOL_EXPORT NO_NULL bool harbol_array_shift_up(struct HarbolArray *const vec, const size_t index, const size_t datasize, const size_t amount);

HARBOL_EXPORT NO_NULL size_t harbol_array_item_count(const struct HarbolArray *const vec, const void *const val, const size_t datasize);
HARBOL_EXPORT NO_NULL size_t harbol_array_index_of(const struct HarbolArray *const vec, const void *const val, const size_t datasize, const size_t starting_index);


/// deletion funcs.
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_index(struct HarbolArray *const vec, const size_t index, const size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_range(struct HarbolArray *const vec, const size_t index, const size_t datasize, const size_t range);
HARBOL_EXPORT NO_NULL bool harbol_array_del_by_val(struct HarbolArray *const vec, const void *const val, const size_t datasize);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_ARRAY_INCLUDED */