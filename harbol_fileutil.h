#ifndef HARBOL_FILEUTIL_INCLUDED
#	define HARBOL_FILEUTIL_INCLUDED


HELPER_FUNC NO_NULL ssize_t get_file_size(FILE *const file) {
	fseek(file, 0, SEEK_END);
	ssize_t const filesize = ftell(file);
	rewind(file);
	return filesize;
}

/// Harbol Iterator.
/// for "struct/union" types, cast from the 'ptr' alias.
union HarbolIter {
	bool             const *b00l;
	uint8_t          const *uint8;      int8_t   const *int8;
	uint16_t         const *uint16;     int16_t  const *int16;
	uint32_t         const *uint32;     int32_t  const *int32;
	uint64_t         const *uint64;     int64_t  const *int64;
	size_t           const *size;       ssize_t  const *ssize;
	uintptr_t        const *uintptr;    intptr_t const *intptr;
	
	float32_t        const *float32;
	float64_t        const *float64;
	floatptr_t       const *floatptr;
	floatmax_t       const *floatmax;
	
	char             const *cstr;
	void             const *ptr;
	union HarbolIter const *self;
};


HELPER_FUNC NO_NULL uint8_t *make_buffer_from_file(FILE *const restrict file, size_t *const restrict bytes) {
	ssize_t const filesize = get_file_size(file);
	if( filesize <= 0 ) {
		return nullptr;
	}
	
	uint8_t *const restrict stream = C_PTR_CAST(stream, calloc(filesize, sizeof *stream));
	if( stream==nullptr ) {
		*bytes = SIZE_MAX;
	} else {
		*bytes = fread(stream, sizeof *stream, filesize, file);
	}
	return stream;
}

HELPER_FUNC NO_NULL uint8_t *make_buffer_from_binary(char const ARRAY_PARAM_FULL(file_name, 1), size_t *const restrict bytes) {
	FILE *restrict file = fopen(file_name, "rb");
	if( file==nullptr ) {
		return nullptr;
	}
	uint8_t *const restrict stream = C_PTR_CAST(stream, make_buffer_from_file(file, bytes));
	fclose(file); file = nullptr;
	return stream;
}

HELPER_FUNC NO_NULL char *make_buffer_from_text(char const ARRAY_PARAM_FULL(file_name, 1), size_t *const restrict len) {
	FILE *restrict file = fopen(file_name, "r");
	if( file==nullptr ) {
		return nullptr;
	}
	char *const restrict stream = C_PTR_CAST(stream, make_buffer_from_file(file, len));
	fclose(file); file = nullptr;
	return stream;
}

#endif /** HARBOL_FILEUTIL_INCLUDED */