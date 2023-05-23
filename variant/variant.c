#include "variant.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

HARBOL_EXPORT struct HarbolVariant *harbol_variant_new(void const *const val, size_t const datasize, int32_t const type_flags) {
	struct HarbolVariant *restrict variant = calloc(1, sizeof *variant);
	if( variant==NULL || !harbol_variant_init(variant, val, datasize, type_flags) ) {
		free(variant); variant = NULL;
	}
	return variant;
}

HARBOL_EXPORT struct HarbolVariant harbol_variant_make(void const *const val, size_t const datasize, int32_t const type_flags, bool *const restrict res) {
	struct HarbolVariant v = {0};
	*res = harbol_variant_init(&v, val, datasize, type_flags);
	return v;
}

HARBOL_EXPORT bool harbol_variant_init(struct HarbolVariant *const restrict variant, void const *const val, size_t const datasize, int32_t const type_flags) {
	variant->data = dup_data(val, datasize);
	if( variant->data==NULL ) {
		return false;
	}
	variant->size = datasize;
	variant->tag = type_flags;
	return true;
}

HARBOL_EXPORT void harbol_variant_clear(struct HarbolVariant *const variant) {
	free(variant->data); variant->data=NULL;
	variant->size = 0;
	variant->tag = 0;
}

HARBOL_EXPORT void harbol_variant_free(struct HarbolVariant **const variantref) {
	if( *variantref==NULL ) {
		return;
	}
	harbol_variant_clear(*variantref);
	free(*variantref); *variantref = NULL;
}

HARBOL_EXPORT void *harbol_variant_data(struct HarbolVariant const *const variant) {
	return variant->data;
}

HARBOL_EXPORT size_t harbol_variant_size(struct HarbolVariant const *const variant) {
	return variant->size;
}

HARBOL_EXPORT int32_t harbol_variant_tag(struct HarbolVariant const *const variant) {
	return variant->tag;
}

HARBOL_EXPORT bool harbol_variant_set(struct HarbolVariant *const restrict variant, void const *const val, size_t const datasize) {
	uint8_t *const cpy = dup_data(val, datasize);
	if( cpy==NULL ) {
		return false;
	}
	if( variant->data != NULL ) {
		free(variant->data); variant->data = NULL;
	}
	variant->data = cpy;
	variant->size = datasize;
	return true;
}
