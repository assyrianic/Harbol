#ifndef HARBOL_TUPLE_INCLUDED
#	define HARBOL_TUPLE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../array/array.h"


struct HarbolTuple {
	struct HarbolArray fields;
	uint8_t           *datum;
	size_t             len;
	bool               packed : 1;
};


HARBOL_EXPORT NO_NULL struct HarbolTuple *harbol_tuple_new(size_t len, size_t const sizes[], bool packed);
HARBOL_EXPORT NO_NULL struct HarbolTuple harbol_tuple_create(size_t len, size_t const sizes[], bool packed, bool *res);
HARBOL_EXPORT NO_NULL bool harbol_tuple_init(struct HarbolTuple *tuple, size_t len, size_t const sizes[], bool packed);

HARBOL_EXPORT NO_NULL void harbol_tuple_clear(struct HarbolTuple *tuple);
HARBOL_EXPORT NO_NULL void harbol_tuple_free(struct HarbolTuple **tupleref);

HARBOL_EXPORT NO_NULL size_t harbol_tuple_len(struct HarbolTuple const *tuple);
HARBOL_EXPORT NO_NULL size_t harbol_tuple_fields(struct HarbolTuple const *tuple);

HARBOL_EXPORT NO_NULL void *harbol_tuple_get(struct HarbolTuple const *tuple, size_t index);
HARBOL_EXPORT NO_NULL void *harbol_tuple_set(struct HarbolTuple const *tuple, size_t index, void *field);

HARBOL_EXPORT NO_NULL size_t harbol_tuple_field_size(struct HarbolTuple const *tuple, size_t index);
HARBOL_EXPORT NO_NULL bool harbol_tuple_packed(struct HarbolTuple const *tuple);
HARBOL_EXPORT NO_NULL bool harbol_tuple_to_struct(struct HarbolTuple const *tuple, void *struc);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_TUPLE_INCLUDED */
