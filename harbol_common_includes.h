#ifndef HARBOL_COMMON_INCLUDES_INCLUDED
#	define HARBOL_COMMON_INCLUDES_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>

#ifdef __cplusplus
#	include <type_traits>
#endif


/** placing this here so we can get this after including inttypes.h */
#if defined(INTPTR_MAX)
#	if defined(INT32_MAX) && INTPTR_MAX==INT32_MAX
#		ifndef HARBOL32
#			define HARBOL32
#		endif
#	endif
#	if defined(INT64_MAX) && INTPTR_MAX==INT64_MAX
#		ifndef HARBOL64
#			define HARBOL64
#		endif
#	endif
#endif


#ifndef NIL
#	define NIL    ( uintptr_t )(NULL)
#endif


/** types as defined by Harbol. */
#ifndef __ssize_t_defined
#	if SIZE_MAX==UINT_MAX
		typedef int ssize_t;        /** common 32 bit case */
#		define SSIZE_MIN  INT_MIN
#		define SSIZE_MAX  INT_MAX
#	elif SIZE_MAX==ULONG_MAX
		typedef long ssize_t;       /** linux 64 bits */
#		define SSIZE_MIN  LONG_MIN
#		define SSIZE_MAX  LONG_MAX
#	elif SIZE_MAX==ULLONG_MAX
		typedef long long ssize_t;  /** windows 64 bits */
#		define SSIZE_MIN  LLONG_MIN
#		define SSIZE_MAX  LLONG_MAX
#	endif
#	define __ssize_t_defined
#endif


/** According to C99 standards.
 * there are three floating point types: float, double, and long double.
 * 
 * The type double provides at least as much precision as float, and the type long double provides at least as much precision as double.
 * so in summary: float <= double <= long double
 */
#ifndef __float32_t_defined
#	if FLT_MANT_DIG==24
#		define __float32_t_defined
#		define PRIf32    "f"
#		define SCNf32    "f"
#		define SCNxf32   "a"
#		define strtof32  strtof
		typedef float    float32_t;
#	elif DBL_MANT_DIG==24
#		define __float32_t_defined
#		define PRIf32    "f"
#		define SCNf32    "lf"
#		define SCNxf32   "la"
#		define strtof32  strtod
		typedef double   float32_t;
#	else
#		error "no appropriate float32_t implementation"
#	endif
#endif

#ifdef C11
	_Static_assert(sizeof(float32_t) * CHAR_BIT == 32, "Unexpected `float32_t` size");
#endif


#ifndef __float64_t_defined
#	if DBL_MANT_DIG==53
#		define __float64_t_defined
#		define PRIf64    "f"
#		define SCNf64    "lf"
#		define SCNxf64   "la"
#		define strtof64  strtod
		typedef double   float64_t;
#	elif LDBL_MANT_DIG==53
#		define __float64_t_defined
#		define PRIf64    "Lf"
#		define SCNf64    "Lf"
#		define SCNxf64   "La"
#		define strtof64  strtold
		typedef long double float64_t;
/// This is unlikely but check just in case.
#	elif FLT_MANT_DIG==53
#		define __float64_t_defined
#		define PRIf64    "f"
#		define SCNf64    "f"
#		define SCNxf64   "a"
#		define strtof64  strtof
		typedef float    float64_t;
#	else
#		error "no appropriate float64_t implementation"
#	endif
#endif

#ifdef C11
	_Static_assert(sizeof(float64_t) * CHAR_BIT == 64, "Unexpected `float64_t` size");
#endif


#ifndef __floatptr_t_defined
#	if defined(HARBOL64)
#		define __floatptr_t_defined
#		define PRIfPTR    PRIf64
#		define strtofptr  strtof64
#		define SCNfPTR    SCNf64
#		define SCNxfPTR   SCNxf64
		typedef float64_t floatptr_t;
#	elif defined(HARBOL32)
#		define __floatptr_t_defined
#		define PRIfPTR    PRIf32
#		define strtofptr  strtof32
#		define SCNfPTR    SCNf32
#		define SCNxfPTR   SCNxf32
		typedef float32_t floatptr_t;
#	else
#		error "no appropriate floatptr_t implementation"
#	endif
#endif

#ifdef C11
	_Static_assert(sizeof(floatptr_t)==sizeof(intptr_t), "Unexpected `floatptr_t` size");
#endif


#ifndef __floatmax_t_defined
#	if LDBL_MANT_DIG > DBL_MANT_DIG
#		define __floatmax_t_defined
#		define PRIfMAX    "Lf"
#		define SCNfMAX    "Lf"
#		define SCNxfMAX   "La"
#		define strtofmax  strtold
		typedef long double floatmax_t;
#	elif DBL_MANT_DIG==LDBL_MANT_DIG && DBL_MANT_DIG > FLT_MANT_DIG
#		define __floatmax_t_defined
#		define PRIfMAX    "f"
#		define SCNfMAX    "lf"
#		define SCNxfMAX   "la"
#		define strtofmax  strtod
		typedef double    floatmax_t;
#	elif DBL_MANT_DIG==FLT_MANT_DIG
#		define __floatmax_t_defined
#		define PRIfMAX    "f"
#		define SCNfMAX    "f"
#		define SCNxfMAX   "a"
#		define strtofmax  strtof
		typedef float     floatmax_t;
#	else
#		error "no appropriate floatmax_t implementation"
#	endif
#endif


static inline void *harbol_recalloc(void *const arr, size_t const new_size, size_t const element_size, size_t const old_size) {
	if( arr==NULL || old_size==0 ) {
		return calloc(new_size, element_size);
	}
#ifdef __cplusplus
	uint8_t *const restrict new_block = reinterpret_cast< decltype(new_block) >(realloc(arr, new_size * element_size));
#else
	uint8_t *const restrict new_block = realloc(arr, new_size * element_size);
#endif
	if( new_block==NULL ) {
		return NULL;
	} else if( old_size < new_size ) {
		memset(&new_block[old_size * element_size], 0, (new_size - old_size) * element_size);
	}
	return new_block;
}

#ifdef __cplusplus
template< typename T >
static inline T *harbol_tg_recalloc(T *const arr, size_t const new_size, size_t const old_size) {
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


/// This is for allocating multiple buffers that all have to be the same size.
/// use like: 'harbol_multi_calloc(size, num_bufs, &buf1, sizeof *buf1, ...);'
/// If allocation fails, all buffers are freed.
static inline bool harbol_multi_calloc(size_t const size, size_t const n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	va_list cp; va_copy(cp, ap);
	bool res = true;
	for( size_t i=0; i < n_buffers; i++ ) {
		void       **buf       = va_arg(ap, void**);
		size_t const elem_size = va_arg(ap, size_t);
		*buf = calloc(size, elem_size);
		res &= *buf != NULL;
	}
	va_end(ap);
	
	if( !res ) {
		for( size_t i=0; i < n_buffers; i++ ) {
			void **buf = va_arg(cp, void**);
			va_arg(cp, size_t);
			free(*buf); *buf = NULL;
		}
	}
	va_end(cp);
	return res;
}


/// Works similar to `harbol_multi_calloc` but for `recalloc`ing instead.
/// use like: 'harbol_multi_calloc(newsize, oldsize, num_bufs, &buf1, sizeof *buf1, ...);'
/// If allocation fails, all given buffers are unchanged.
static inline bool harbol_multi_recalloc(size_t const new_size, size_t const old_size, size_t const n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	va_list cp; va_copy(cp, ap);
	bool res = true;
	void **bufs = calloc(n_buffers, sizeof *bufs);
	for( size_t i=0; i < n_buffers; i++ ) {
		void **buf = va_arg(ap, void**);
		size_t const elem_size = va_arg(ap, size_t);
		bufs[i] = harbol_recalloc(*buf, new_size, elem_size, old_size);
		res &= bufs[i] != NULL;
	}
	va_end(ap);
	
	if( !res ) {
		for( size_t i=0; i < n_buffers; i++ ) {
			free(bufs[i]); bufs[i] = NULL;
		}
	} else {
		for( size_t i=0; i < n_buffers; i++ ) {
			void **buf = va_arg(cp, void**);
			va_arg(cp, size_t);
			*buf = bufs[i];
		}
	}
	free(bufs); bufs = NULL;
	va_end(cp);
	return res;
}


static inline void harbol_cleanup(void *const ptr_ref) {
#ifdef __cplusplus
	void **const p = reinterpret_cast< decltype(p) >(ptr_ref);
#else
	void **const p = ptr_ref;
#endif
	free(*p); *p = NULL;
}

/// Works like to `harbol_cleanup`.
/// use like: 'harbol_multi_cleanup(num_bufs, &buf1, &buf2, &buf3, ...);'
static inline void harbol_multi_cleanup(size_t const n_buffers, ...) {
	va_list ap; va_start(ap, n_buffers);
	for( size_t i=0; i < n_buffers; i++ ) {
		void **buf = va_arg(ap, void**);
		if( buf==NULL ) {
			break;
		}
		free(*buf); *buf = NULL;
	}
	va_end(ap);
}


static inline NO_NULL void *harbol_mempcpy(void *const dest, void const *const src, size_t const bytes) {
#ifdef __cplusplus
	uint8_t *const r = reinterpret_cast< decltype(r) >(memcpy(dest, src, bytes));
	return r + bytes;
#else
	return( ( uint8_t* )(memcpy(dest, src, bytes)) + bytes );
#endif
}

static inline NO_NULL void *harbol_memccpy(void *const restrict dest, void const *const src, int const c, size_t const bytes) {
#ifdef __cplusplus
	uint8_t const *const p = reinterpret_cast< decltype(p) >(memchr(src, c, bytes));
	if( p != nullptr ) {
		uint8_t const *const s = reinterpret_cast< decltype(s) >(src);
		return harbol_mempcpy(dest, src, (p - s + 1));
	}
	memcpy(dest, src, bytes);
	return nullptr;
#else
	uint8_t const *const p = memchr(src, c, bytes);
	if( p != NULL ) {
		return harbol_mempcpy(dest, src, (p - ( uint8_t const* )(src) + 1));
	}
	memcpy(dest, src, bytes);
	return NULL;
#endif
}


static inline size_t harbol_align_size(size_t const size, size_t const align) {
	return (size + (align - 1)) & ~(align - 1);
}

static inline size_t harbol_pad_size(size_t const size, size_t const align) {
	return (align - (size & (align - 1))) & (align - 1);
}


/// these are NOT cryptographic hashes.
/// use ONLY FOR HASH TABLE IMPLEMENTATIONS.
/// The implementation is a [largely unmodified] SDBM algorithm
#ifdef __cplusplus
static inline NO_NULL size_t string_hash(char const *const key, size_t const seed=0)
#else
static inline size_t string_hash(char const key[const static 1], size_t const seed)
#endif
{
	size_t h = seed;
	for( size_t i=0; key[i] != 0; i++ ) {
		h = ( size_t )(key[i]) + (h << 6) + (h << 16) - h;
	}
	return h;
}


#ifdef __cplusplus
static inline NO_NULL size_t array_hash(uint8_t const *const key, size_t const len, size_t const seed=0)
#else
static inline size_t array_hash(uint8_t const key[const static 1], size_t const len, size_t const seed)
#endif
{
	size_t h = seed;
	for( size_t i=0; i < len; i++ ) {
		h = ( size_t )(key[i]) + (h << 6) + (h << 16) - h;
	}
	return h;
}

static inline size_t int_hash(size_t const i, size_t const seed) {
	size_t h = seed;
	for( size_t n=0; n < (sizeof(size_t) * CHAR_BIT); n += 8 ) {
		h = ((i >> n) & 0xFF) + (h << 6) + (h << 16) - h;
	}
	return h;
}

static inline size_t float_hash(floatptr_t const a, size_t const seed) {
	union {
		floatptr_t const f;
		size_t     const s;
	} c = {a};
	return int_hash(c.s, seed);
}

static inline NO_NULL size_t ptr_hash(void const *const p, size_t const seed) {
	union {
		void const *const p;
		size_t const      y;
	} c = {p};
	return (c.y >> 4u) | ((c.y << (8u * sizeof(void*) - 4u)) + seed);
}

#ifdef C11
#	define harbol_hash(h)   _Generic((h)+0, \
				int         : int_hash,     \
				size_t      : int_hash,     \
				int64_t     : int_hash,     \
				uint64_t    : int_hash,     \
				float32_t   : float_hash,   \
				float64_t   : float_hash,   \
				floatptr_t  : float_hash,   \
				char*       : string_hash,  \
				char const* : string_hash,  \
				default     : ptr_hash)     \
							((h))
#endif


static inline NO_NULL ssize_t get_file_size(FILE *const file) {
	fseek(file, 0, SEEK_END);
	ssize_t const filesize = ftell(file);
	rewind(file);
	return filesize;
}

/// Harbol Iterator.
/// for "struct/union" types, cast from the 'ptr' alias.
union HarbolIter {
	bool      const *b00l;
	uint8_t   const *uint8;   int8_t   const *int8;
	uint16_t  const *uint16;  int16_t  const *int16;
	uint32_t  const *uint32;  int32_t  const *int32;
	uint64_t  const *uint64;  int64_t  const *int64;
	size_t    const *size;    ssize_t  const *ssize;
	uintptr_t const *uintptr; intptr_t const *intptr;
	
	float32_t  const         *float32;
	float64_t  const         *float64;
	floatptr_t const         *floatptr;
	floatmax_t const         *floatmax;
	
	char             const   *string;
	void             const   *ptr;
	union HarbolIter const   *self;
};

#ifdef __cplusplus
static inline NO_NULL uint8_t *make_buffer_from_binary(char const *const restrict file_name, size_t *const restrict bytes)
#else
static inline NO_NULL uint8_t *make_buffer_from_binary(char const file_name[const restrict static 1], size_t *const restrict bytes)
#endif
{
	FILE *restrict file = fopen(file_name, "rb");
	if( file==NULL ) {
		return NULL;
	}
	
	ssize_t const filesize = get_file_size(file);
	if( filesize <= 0 ) {
		fclose(file);
		return NULL;
	}
	
#ifdef __cplusplus
	uint8_t *restrict stream = reinterpret_cast< decltype(stream) >(calloc(filesize, sizeof *stream));
#else
	uint8_t *restrict stream = calloc(filesize, sizeof *stream);
#endif
	*bytes = fread(stream, sizeof *stream, filesize, file);
	fclose(file); file = NULL;
	return stream;
}

#ifdef __cplusplus
static inline NO_NULL char *make_buffer_from_text(char const *const restrict file_name, size_t *const restrict len)
#else
static inline NO_NULL char *make_buffer_from_text(char const file_name[const restrict static 1], size_t *const restrict len)
#endif
{
	FILE *restrict file = fopen(file_name, "r");
	if( file==NULL ) {
		return NULL;
	}
	
	ssize_t const filesize = get_file_size(file);
	if( filesize <= 0 ) {
		fclose(file);
		return NULL;
	}
	
#ifdef __cplusplus
	char *restrict stream = reinterpret_cast< decltype(stream) >(calloc(filesize + 1, sizeof *stream));
#else
	char *restrict stream = calloc(filesize + 1, sizeof *stream);
#endif
	*len = fread(stream, sizeof *stream, filesize, file);
	fclose(file); file = NULL;
	return stream;
}

static inline bool is_ptr_aligned(void const *const ptr, size_t const bytes) {
	return (( uintptr_t )(ptr) & (bytes-1))==0;
}

static inline NO_NULL void *dup_data(void const *const restrict data, size_t const bytes) {
#ifdef __cplusplus
	uint8_t *restrict cpy = reinterpret_cast< decltype(cpy) >(calloc(bytes, sizeof *cpy));
#else
	uint8_t *restrict cpy = calloc(bytes, sizeof *cpy);
#endif
	return( cpy==NULL )? NULL : memcpy(cpy, data, bytes);
}

#ifdef __cplusplus
static inline NO_NULL char *dup_cstr(size_t const len, char const *cstr)
#else
static inline NO_NULL char *dup_cstr(size_t const len, char const cstr[const restrict static len])
#endif
{
#ifdef __cplusplus
	char *restrict cpy = reinterpret_cast< decltype(cpy) >(calloc(len + 1, sizeof *cpy));
#else
	char *restrict cpy = calloc(len + 1, sizeof *cpy);
#endif
	return( cpy==NULL )? NULL : strcpy(cpy, cstr);
}


#ifdef __cplusplus
static inline NO_NULL char *sprintf_alloc(char const *const restrict fmt, ...)
#else
static inline NO_NULL char *sprintf_alloc(char const fmt[const restrict static 1], ...)
#endif
{
	va_list ap; va_start(ap, fmt);
	va_list st; va_copy(st, ap);
	
	char c = 0;
	int32_t const size = vsnprintf(&c, 1, fmt, ap);
	va_end(ap);
	
#ifdef __cplusplus
	char *restrict text = reinterpret_cast< decltype(cpy) >(calloc(size + 2, sizeof *text));
#else
	char *restrict text = calloc(size + 2, sizeof *text);
#endif
	if( text != NULL ) {
		vsnprintf(text, size + 1, fmt, st);
	}
	va_end(st);
	return text;
}

static inline bool is_uint_in_bounds(size_t const val, size_t const max, size_t const min) {
	return (val - min) <= (max - min);
}

static inline bool is_int_in_bounds(ssize_t const val, ssize_t const max, ssize_t const min) {
	return is_uint_in_bounds(( size_t )(val), ( size_t )(max), ( size_t )(min));
}

static inline NO_NULL bool is_uintptr_in_bounds(uintptr_t const val, uintptr_t const max, uintptr_t const min) {
	return (val - min) <= (max - min);
}


/// END the params with a NULL.
/// Within a variable list of strings to compare to,
/// returns an index or SIZE_MAX aka (size_t)(-1) if not found
#ifdef __cplusplus
static inline size_t cstr_switch(char const *cstr, ...) {
#else
static inline size_t cstr_switch(char const cstr[static 1], ...) {
#endif
	va_list ap; va_start(ap, cstr);
	size_t index = 0;
	char const *arg = va_arg(ap, char const*);
	while( arg != NULL ) {
		if( !strcmp(cstr, arg) ) {
			break;
		}
		index++;
		arg = va_arg(ap, char const *);
	}
	va_end(ap);
	return arg==NULL? SIZE_MAX : index;
}

static inline bool array_shift_up(void *const restrict buf, size_t *const restrict len, size_t const index, size_t const datasize, size_t amount) {
	if( index >= *len ) {
		return false;
	}
	
	amount += amount==0;
	size_t const
		i = index + amount,
		j = index
	;
	uint8_t *const restrict b = buf;
	if( i < *len ) {
		*len -= amount;
		memmove(&b[j * datasize], &b[i * datasize], (*len - j) * datasize);
		memset(&b[*len * datasize], 0, amount * datasize);
	} else {
		/// if i goes out of range, zero everything after and lower the count.
		memset(&b[j * datasize], 0, (*len - j) * datasize);
		*len = j;
	}
	return true;
}


/// This is for shifting multiple buffers that are controlled by a single length size.
/// use like: 'multi_array_shift_up(&len, index_to_shift_from, amount_to_shift, num_buffers, buf1, sizeof *buf1, ...);'
/// perfect for use with parallel arrays/buffers.
static inline bool multi_array_shift_up(size_t *const restrict len, size_t const index, size_t amount, size_t const n_buffers, ...) {
	if( index >= *len ) {
		return false;
	}
	
	va_list ap; va_start(ap, n_buffers);
	amount += amount==0;
	size_t const
		i = index + amount,
		j = index
	;
	
	if( i < *len ) {
		*len -= amount;
		size_t const cur_len = *len;
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const buf      = va_arg(ap, uint8_t*);
			size_t   const datasize = va_arg(ap, size_t);
			memmove(&buf[j * datasize], &buf[i * datasize], (cur_len - j) * datasize);
			memset(&buf[cur_len * datasize], 0, amount * datasize);
		}
	} else {
		size_t const cur_len = *len;
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const restrict buf      = va_arg(ap, uint8_t*);
			size_t   const          datasize = va_arg(ap, size_t);
			memset(&buf[j * datasize], 0, (cur_len - j) * datasize);
		}
		*len = j;
	}
	va_end(ap);
	return true;
}

static inline size_t next_pow_of_2(size_t x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
#if SIZE_MAX==UINT64_MAX
	x |= x >> 32;
#endif
	return x + 1;
}

/// Saving here for future components of Harbol.
/*
static inline bool yes_or_no(FILE *const restrict output, char const *const restrict msg, char const *const restrict yes) {
	fputs(msg, output);
	char letter[5] = {0};
	fgets(letter, sizeof letter, stdin);
	return !strcmp(letter, yes);
}
*/

#endif /** HARBOL_COMMON_INCLUDES_INCLUDED */