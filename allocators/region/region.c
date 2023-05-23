#include "region.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolRegion harbol_region_make(size_t const size) {
	struct HarbolRegion region = {0};
	if( size==0 ) {
		return region;
	}
	
	region.mem = calloc(size, sizeof *region.mem);
	if( region.mem==NULL ) {
		return region;
	}
	
	region.size = region.offs = size;
	return region;
}

HARBOL_EXPORT struct HarbolRegion harbol_region_make_from_buffer(void *const buf, size_t const size) {
	struct HarbolRegion region = {0};
	if( size==0 ) {
		return region;
	}
	region.size = region.offs = size;
	region.mem  = buf;
	return region;
}

HARBOL_EXPORT void harbol_region_clear(struct HarbolRegion *const region) {
	if( region->mem==NULL ) {
		return;
	}
	free(region->mem);
	*region = ( struct HarbolRegion ){0};
}

HARBOL_EXPORT void *harbol_region_alloc(struct HarbolRegion *const region, size_t const size) {
	if( region->mem==NULL || size==0 || size > region->size ) {
		return NULL;
	}
	
	size_t const alloc_size = harbol_align_size(size, sizeof(uintptr_t));
	if( region->offs - alloc_size >= region->size ) {
		return NULL;
	}
	region->offs -= alloc_size;
	return memset(region->mem + region->offs, 0, alloc_size);
}

HARBOL_EXPORT size_t harbol_region_remaining(struct HarbolRegion const *const region) {
	return region->offs > region->size? 0 : region->offs;
}
