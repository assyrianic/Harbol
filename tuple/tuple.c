#include "tuple.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

HARBOL_EXPORT struct HarbolTuple harbol_tuple_create(size_t const len, size_t const sizes[const static 1], bool const packed, bool *const restrict res) {
	struct HarbolTuple tuple = {0};
	*res = harbol_tuple_init(&tuple, len, sizes, packed);
	return tuple;
}

HARBOL_EXPORT struct HarbolTuple *harbol_tuple_new(size_t const len, size_t const sizes[const static 1], bool const packed) {
	struct HarbolTuple *tuple = calloc(1, sizeof *tuple);
	if( tuple==NULL || !harbol_tuple_init(tuple, len, sizes, packed) ) {
		free(tuple); tuple = NULL;
	}
	return tuple;
}

HARBOL_EXPORT bool harbol_tuple_init(struct HarbolTuple *const tuple, size_t const len, size_t const sizes[const static 1], bool const packed) {
	size_t const ptr_size = sizeof(intptr_t);
	size_t largest_memb = 0;
	
	/// first we find the largest member of the tuple:
	for( size_t i=0; i < len; i++ ) {
		if( largest_memb < sizes[i] ) {
			largest_memb = sizes[i];
		}
	}
	
	/// next, compute padding and alignment. we do this by having a next and previous size.
	size_t
		total_size=0,
		prev_size=0
	;
	for( size_t i=0; i < len; i++ ) {
		total_size += sizes[i];
		if( packed || len==1 ) {
			continue;
		}
		size_t const offalign = (i+1 < len)? sizes[i+1] : prev_size;
		total_size = harbol_align_size(total_size, offalign >= ptr_size? ptr_size : offalign);
		prev_size = sizes[i];
	}
	
	/// now do a final size alignment with the largest member.
	size_t const aligned_total = harbol_align_size(total_size, largest_memb >= ptr_size? ptr_size : largest_memb);
	tuple->datum = calloc(packed? total_size : aligned_total, sizeof *tuple->datum);
	if( tuple->datum==NULL ) {
		return false;
	}
	
	tuple->len = packed? total_size : aligned_total;
	tuple->fields = harbol_array_make(sizeof(uint32_t), tuple->len, &( bool ){false});
	uint32_t offset = 0;
	for( size_t i=0; i < len; i++ ) {
		uint32_t const size_pack = (( uint32_t )(sizes[i]) << 16) | offset;
		harbol_array_insert(&tuple->fields, &size_pack, sizeof size_pack);
		offset += sizes[i];
		if( packed || len==1 ) {
			continue;
		}
		size_t const offalign = (i+1<len)? sizes[i+1] : prev_size;
		offset = harbol_align_size(offset, offalign >= ptr_size? ptr_size : offalign);
		prev_size = sizes[i];
	}
	return true;
}

HARBOL_EXPORT void harbol_tuple_clear(struct HarbolTuple *const tuple) {
	harbol_array_clear(&tuple->fields);
	free(tuple->datum); tuple->datum = NULL;
	tuple->len = 0;
}

HARBOL_EXPORT void harbol_tuple_free(struct HarbolTuple **tupleref) {
	if( *tupleref==NULL ) {
		return;
	}
	harbol_tuple_clear(*tupleref);
	free(*tupleref); *tupleref = NULL;
}

HARBOL_EXPORT size_t harbol_tuple_len(struct HarbolTuple const *const tuple) {
	return tuple->len;
}

HARBOL_EXPORT size_t harbol_tuple_fields(struct HarbolTuple const *const tuple) {
	return tuple->fields.len;
}

HARBOL_EXPORT void *harbol_tuple_get(struct HarbolTuple const *const tuple, size_t const index) {
	if( tuple->datum==NULL || tuple->len==0 ) {
		return NULL;
	}
	
	uint32_t const *const field_data = harbol_array_get(&tuple->fields, index, sizeof *field_data);
	return( field_data==NULL || (*field_data & 0xFFFF) >= tuple->len )? NULL : tuple->datum + (*field_data & 0xFFFF);
}

HARBOL_EXPORT void *harbol_tuple_set(struct HarbolTuple const *const tuple, size_t const index, void *const val) {
	if( tuple->datum==NULL || tuple->len==0 ) {
		return NULL;
	}
	
	void *const restrict field = harbol_tuple_get(tuple, index);
	if( field==NULL ) {
		return NULL;
	}
	
	uint32_t const *const field_data = harbol_array_get(&tuple->fields, index, sizeof *field_data);
	return memcpy(field, val, *field_data >> 16);
}

HARBOL_EXPORT size_t harbol_tuple_field_size(struct HarbolTuple const *const tuple, size_t const index) {
	if( tuple->datum==NULL || tuple->len==0 ) {
		return 0;
	}
	uint32_t const *const field_data = harbol_array_get(&tuple->fields, index, sizeof *field_data);
	return( field_data==NULL )? 0 : *field_data >> 16;
}

HARBOL_EXPORT bool harbol_tuple_packed(struct HarbolTuple const *const tuple) {
	return tuple->packed;
}

HARBOL_EXPORT bool harbol_tuple_to_struct(struct HarbolTuple const *const tuple, void *const restrict struc) {
	if( tuple->datum==NULL || tuple->len==0 ) {
		return false;
	}
	memcpy(struc, tuple->datum, tuple->len);
	return true;
}
