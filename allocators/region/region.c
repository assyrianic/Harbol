#include "region.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolRegion harbol_region_make(void *const buffer, size_t const size, bool *const restrict res) {
	struct HarbolRegion region = {0};
	if( size==0 ) {
		return region;
	}
	
	region.mem = ( buffer==nullptr )? calloc(size, sizeof *region.mem) : buffer;
	if( region.mem==nullptr ) {
		*res = false;
		return region;
	}
	*res = true;
	region.size = region.offs = size;
	return region;
}

HARBOL_EXPORT void harbol_region_clear(struct HarbolRegion *const region) {
	if( region->mem==nullptr ) {
		return;
	}
	free(region->mem);
	*region = ( struct HarbolRegion ){0};
}

HARBOL_EXPORT void *harbol_region_alloc(struct HarbolRegion *const region, size_t const size) {
	if( region->mem==nullptr || size==0 || size > region->size ) {
		return nullptr;
	}
	
	size_t const alloc_size = harbol_align_size(size, sizeof alloc_size);
	if( region->offs - alloc_size >= region->size ) {
		return nullptr;
	}
	region->offs -= alloc_size;
	return memset(region->mem + region->offs, 0, alloc_size);
}

HARBOL_EXPORT size_t harbol_region_remaining(struct HarbolRegion const *const region) {
	return region->offs > region->size? 0 : region->offs;
}