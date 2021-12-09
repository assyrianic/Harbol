#ifndef HARBOL_VARIANT_INCLUDED
#	define HARBOL_VARIANT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


struct HarbolVariant {
	uint8_t *data;
	size_t   size, align_or_len;
	int32_t  tag;
};


HARBOL_EXPORT NO_NULL struct HarbolVariant harbol_variant_make(const void *val, size_t datasize, int32_t type_flags, bool *res);
HARBOL_EXPORT NO_NULL bool harbol_variant_init(struct HarbolVariant *variant, const void *val, size_t datasize, int32_t type_flags);
HARBOL_EXPORT NO_NULL struct HarbolVariant *harbol_variant_new(const void *val, size_t datasize, int32_t type_flags);

HARBOL_EXPORT NO_NULL void harbol_variant_clear(struct HarbolVariant *variant);
HARBOL_EXPORT NO_NULL void harbol_variant_free(struct HarbolVariant **variantref);

HARBOL_EXPORT NO_NULL void *harbol_variant_data(const struct HarbolVariant *variant);
HARBOL_EXPORT NO_NULL size_t harbol_variant_size(const struct HarbolVariant *variant);
HARBOL_EXPORT NO_NULL int32_t harbol_variant_tag(const struct HarbolVariant *variant);

HARBOL_EXPORT NO_NULL bool harbol_variant_set(struct HarbolVariant *variant, const void *val, size_t datasize);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_VARIANT_INCLUDED */