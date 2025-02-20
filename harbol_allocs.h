#ifndef HARBOL_ALLOCS_INCLUDED
#	define HARBOL_ALLOCS_INCLUDED

HELPER_FUNC void *harbol_recalloc(void *const arr, size_t const new_size, size_t const element_size, size_t const old_size) {
	if( arr==nullptr || old_size==0 ) {
		return calloc(new_size, element_size);
	}
	uint8_t *const restrict new_block = C_PTR_CAST(new_block, realloc(arr, new_size * element_size));
	if( new_block==nullptr ) {
		return nullptr;
	} else if( old_size < new_size ) {
		memset(&new_block[old_size * element_size], 0, (new_size - old_size) * element_size);
	}
	return new_block;
}

#ifdef __cplusplus
template< typename T >
HELPER_FUNC T *harbol_tg_recalloc(T *const arr, size_t const new_size, size_t const old_size) {
	if( arr==nullptr || old_size==0 ) {
		return ( T* )(calloc(new_size, sizeof *arr));
	}
	
	T *const restrict new_block = reinterpret_cast< T* >(realloc(arr, new_size * sizeof *arr));
	if( new_block==nullptr ) {
		return nullptr;
	} else if( old_size < new_size ) {
		memset(&new_block[old_size], 0, (new_size - old_size) * sizeof *new_block);
	}
	return new_block;
}
#endif



HELPER_FUNC void _harbol_multi_cleanup_null(va_list va) {
	for( void **buf = va_arg(va, void**); buf != nullptr; buf = va_arg(va, void**) ) {
		free(*buf);
		*buf = nullptr;
	}
}

HELPER_FUNC void _harbol_multi_cleanup_fixed(size_t const n, bool const skip_size, va_list va) {
	for( size_t i=0; i < n; i++ ) {
		void **buf = va_arg(va, void**);
		if( skip_size ) {
			va_arg(va, size_t);
		}
		if( buf==nullptr ) {
			continue;
		}
		free(*buf); *buf = nullptr;
	}
}

/// Works like to `harbol_cleanup`.
/// use like: 'harbol_multi_cleanup(num_bufs, &buf1, &buf2, &buf3, ...);'
HELPER_FUNC void harbol_multi_cleanup(size_t const n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	if( n_buffers==0 ) {
		_harbol_multi_cleanup_null(ap);
	} else {
		_harbol_multi_cleanup_fixed(n_buffers, false, ap);
	}
	va_end(ap);
}


HELPER_FUNC bool _harbol_multi_calloc_null(size_t const size, va_list va) {
	bool res = true;
	for( void **buf = va_arg(va, void**); buf != nullptr; buf = va_arg(va, void**) ) {
		size_t const elem_size = va_arg(va, size_t);
		*buf = calloc(size, elem_size);
		res &= *buf != nullptr;
	}
	return res;
}

HELPER_FUNC bool _harbol_multi_calloc_fixed(size_t const size, size_t const n, va_list va) {
	bool res = true;
	for( size_t i=0; i < n; i++ ) {
		void **buf = va_arg(va, void**);
		size_t const elem_size = va_arg(va, size_t);
		*buf = calloc(size, elem_size);
		res &= *buf != nullptr;
	}
	return res;
}

/// This is for allocating multiple buffers that all have to be the same size.
/// use like: 'harbol_multi_calloc(size, num_bufs, &buf1, sizeof *buf1, ...);'
/// If allocation fails, all buffers are freed.
HELPER_FUNC bool harbol_multi_calloc(size_t const size, size_t const n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	bool const res = ( n_buffers==0 )?
		_harbol_multi_calloc_null(size, ap) : _harbol_multi_calloc_fixed(size, n_buffers, ap);
	
	if( !res ) {
		va_start(ap, n_buffers);
		if( n_buffers==0 ) {
			_harbol_multi_cleanup_null(ap);
		} else {
			_harbol_multi_cleanup_fixed(n_buffers, true, ap);
		}
	}
	va_end(ap);
	return res;
}


HELPER_FUNC size_t _harbol_count_va_buffers(va_list va) {
	size_t n = 0;
	for( void *buf = va_arg(va, void*); buf != nullptr; buf = va_arg(va, void*) ) {
		va_arg(va, size_t);
		n++;
	}
	return n;
}

/// Works similar to `harbol_multi_calloc` but for `recalloc`ing instead.
/// use like: 'harbol_multi_calloc(newsize, oldsize, num_bufs, &buf1, sizeof *buf1, ...);'
/// If allocation fails, all given buffers are unchanged.
HELPER_FUNC bool harbol_multi_recalloc(size_t const new_size, size_t const old_size, size_t n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	
	/// hard to recalloc as we need an array of pointers.
	/// if dev wants flexible size, gotta get number of buffers until nullptr.
	if( n_buffers==0 ) {
		n_buffers = _harbol_count_va_buffers(ap);
		va_start(ap, n_buffers);
	}
	
	bool res = true;
	void **bufs = calloc(n_buffers, sizeof *bufs);
	for( size_t i=0; i < n_buffers; i++ ) {
		void **buf = va_arg(ap, void**);
		size_t const elem_size = va_arg(ap, size_t);
		bufs[i] = harbol_recalloc(*buf, new_size, elem_size, old_size);
		res &= bufs[i] != nullptr;
	}
	
	if( !res ) {
		for( size_t i=0; i < n_buffers; i++ ) {
			free(bufs[i]); bufs[i] = nullptr;
		}
	} else {
		va_start(ap, n_buffers);
		for( size_t i=0; i < n_buffers; i++ ) {
			void **buf = va_arg(ap, void**);
			va_arg(ap, size_t);
			*buf = bufs[i];
		}
	}
	free(bufs); bufs = nullptr;
	va_end(ap);
	return res;
}


HELPER_FUNC void harbol_cleanup(void *const ptr_ref) {
	void **const p = C_PTR_CAST(p, ptr_ref);
	free(*p); *p = nullptr;
}



/// rework of the Harbol multi-allocs but with arrays
/// of buffers instead of using variadic arguments.
HELPER_FUNC bool harbol_calloc_buffers(size_t const size, size_t const n_buffers, void **ARRAY_PARAM_FULL(buffers, n_buffers), size_t const ARRAY_PARAM_LIM(elem_sizes, n_buffers)) {
	bool res = true;
	for( size_t i=0; i < n_buffers; i++ ) {
		*buffers[i] = calloc(size, elem_sizes[i]);
		res &= *buffers[i] != nullptr;
	}
	
	if( !res ) {
		for( size_t i=0; i < n_buffers; i++ ) {
			free(*buffers[i]);
			*buffers[i] = nullptr;
		}
	}
	return res;
}

HELPER_FUNC bool harbol_recalloc_buffers(size_t const new_size, size_t const old_size, size_t const n_buffers, void **ARRAY_PARAM_FULL(buffers, n_buffers), size_t const ARRAY_PARAM_LIM(elem_sizes, n_buffers)) {
	bool res = true;
	for( size_t i=0; i < n_buffers; i++ ) {
		*buffers[i] = harbol_recalloc(*buffers[i], new_size, elem_sizes[i], old_size);
		res &= *buffers[i] != nullptr;
	}
	
	if( !res ) {
		for( size_t i=0; i < n_buffers; i++ ) {
			free(*buffers[i]);
			*buffers[i] = nullptr;
		}
	}
	return res;
}

HELPER_FUNC void harbol_cleanup_buffers(size_t const n_buffers, void **ARRAY_PARAM_FULL(buffers, n_buffers)) {
	for( size_t i=0; i < n_buffers; i++ ) {
		if( buffers[i]==nullptr ) {
			continue;
		}
		free(*buffers[i]);
		*buffers[i] = nullptr;
	}
}

HELPER_FUNC NO_NULL void *dup_data(void const *const restrict data, size_t const bytes) {
	uint8_t *const restrict cpy = C_PTR_CAST(cpy, calloc(bytes, sizeof *cpy));
	return( cpy==nullptr )? nullptr : memcpy(cpy, data, bytes);
}


#ifdef C23
#	ifdef dup_cstr
#		undef dup_cstr
#	endif
#	define dup_cstr(len, cstr)    strndup(cstr, len)
#else
HELPER_FUNC NO_NULL char *dup_cstr(size_t const len, char const ARRAY_PARAM_FULL(cstr, len)) {
	char *const restrict cpy = C_PTR_CAST(cpy, calloc(len + 1, sizeof *cpy));
	return( cpy==nullptr )? nullptr : strcpy(cpy, cstr);
}
#endif

HELPER_FUNC NO_NULL char *sprintf_alloc(char const ARRAY_PARAM_FULL(fmt, 4), ...) {
	va_list ap; va_start(ap, fmt);
	int const size = vsnprintf(nullptr, 0, fmt, ap);
	if( size <= 0 ) {
		return nullptr;
	}
	
	char *const restrict text = C_PTR_CAST(text, calloc(size + 2, sizeof *text));
	if( text != nullptr ) {
		va_start(ap, fmt);
		vsnprintf(text, size + 1, fmt, ap);
	}
	va_end(ap);
	return text;
}

#endif /** HARBOL_ALLOCS_INCLUDED */