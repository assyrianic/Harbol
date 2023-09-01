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
#include <ctype.h>
#include <tgmath.h>

#ifdef __cplusplus
#	include <type_traits>
#endif


/** placing this here so we can get this after including inttypes.h */
#if defined(SIZE_MAX)
#	if defined(UINT32_MAX) && SIZE_MAX==UINT32_MAX
#		ifndef HARBOL32
#			define HARBOL32
#		endif
#	endif
#	if defined(UINT64_MAX) && SIZE_MAX==UINT64_MAX
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

static inline uint8_t *harbol_resize_string(uint8_t *const restrict cstr, size_t const bytes, size_t *const restrict len, size_t const new_size) {
	uint8_t *restrict new_cstr = harbol_recalloc(cstr, new_size + 1, bytes, *len);
	if( new_cstr==NULL ) {
		return NULL;
	}
	*len = new_size;
	return new_cstr;
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
	} const c = {a};
	return int_hash(c.s, seed);
}

static inline NO_NULL size_t ptr_hash(void const *const p, size_t const seed) {
	union {
		void const *const p;
		size_t const      y;
	} const c = {p};
	return (c.y >> 4ul) | ((c.y << (8ul * sizeof(void*) - 4ul)) + seed);
}

#ifdef C11
#	define harbol_hash(h)   _Generic((h)+0, \
				int         : int_hash,     \
				unsigned int: int_hash,     \
				uint32_t    : int_hash,     \
				int32_t     : int_hash,     \
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


static inline NO_NULL uint8_t *make_buffer_from_file(FILE *const restrict file, size_t *const restrict bytes) {
	ssize_t const filesize = get_file_size(file);
	if( filesize <= 0 ) {
		return NULL;
	}
	
#ifdef __cplusplus
	uint8_t *restrict stream = reinterpret_cast< decltype(stream) >(calloc(filesize, sizeof *stream));
#else
	uint8_t *restrict stream = calloc(filesize, sizeof *stream);
#endif
	if( stream==NULL ) {
		*bytes = SIZE_MAX;
	} else {
		*bytes = fread(stream, sizeof *stream, filesize, file);
	}
	return stream;
}

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
	
#ifdef __cplusplus
	uint8_t *restrict stream = reinterpret_cast< decltype(stream) >( make_buffer_from_file(file, bytes) );
#else
	uint8_t *restrict stream = make_buffer_from_file(file, bytes);
#endif
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
	
#ifdef __cplusplus
	char *restrict stream = reinterpret_cast< decltype(stream) >( make_buffer_from_file(file, len) );
#else
	char *restrict stream = ( char* )(make_buffer_from_file(file, len));
#endif
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
static inline size_t cstr_switch(char const *const cstr, ...) {
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
		printf("array_shift_up :: i (%zu) | j (%zu) | '%zu'\n", i, j, amount);
		memmove(&b[j * datasize], &b[i * datasize], (*len - j) * datasize);
		printf("array_shift_up :: b '%s'\n", &b[j * datasize]);
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


static inline size_t bitwise_ceil(size_t x) {
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
#if SIZE_MAX==UINT64_MAX
	x |= x >> 32;
#endif
	return x;
}

static inline size_t next_pow_of_2(size_t const x) {
	return bitwise_ceil(x) + 1;
}

static inline size_t int_log2(size_t const x) {
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	if( x==0 ) {
		return 0;
	}
	return (sizeof x * CHAR_BIT) - __builtin_clzl(x) - 1;
#else
#	if SIZE_MAX==UINT32_MAX
		size_t const de_bruijn_tab[/** 32 */] = {
			 0,  9,  1, 10, 13, 21,  2, 29,
			11, 14, 16, 18, 22, 25,  3, 30,
			 8, 12, 20, 28, 15, 17, 24,  7,
			19, 27, 23,  6, 26,  5,  4, 31
		};
		enum { DeBruijnLog2Magic = 0x07C4ACDDU };
		return de_bruijn_tab[(bitwise_ceil(x) * DeBruijnLog2Magic) >> 27];
#	elif SIZE_MAX==UINT64_MAX
		size_t const de_bruijn_tab[/** 64 */] = {
			0,  58, 1,  59, 47, 53, 2,  60,
			39, 48, 27, 54, 33, 42, 3,  61,
			51, 37, 40, 49, 18, 28, 20, 55,
			30, 34, 11, 43, 14, 22, 4,  62,
			57, 46, 52, 38, 26, 32, 41, 50,
			36, 17, 19, 29, 10, 13, 21, 56,
			45, 25, 31, 35, 16,  9, 12, 44,
			24, 15,  8, 23,  7,  6,  5, 63
		};
		enum { DeBruijnLog2Magic = 0x03F6EAF2CD271461UL };
		return de_bruijn_tab[(bitwise_ceil(x) * DeBruijnLog2Magic) >> 58];
#	else
#		error "no valid SIZE_MAX for `int_log2`."
#	endif
#endif
}

static inline size_t base_2_digits(size_t const x) {
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	return x > 0? (sizeof(size_t) * CHAR_BIT) - __builtin_clzl(x) : 0;
#else
	return int_log2(x) + 1;
#endif
}

static inline size_t base_2_num_chars(size_t const x) {
	return base_2_digits(x) + 1;
}


static inline size_t int_log10(size_t const x) {
	size_t const log10_tab[] = {
		 0,  0,  0,  0,  1,  1,  1,  2,
		 2,  2,  3,  3,  3,  3,  4,  4,
		 4,  5,  5,  5,  6,  6,  6,  6,
		 7,  7,  7,  8,  8,  8,  9,  9,
		 9,  9,
#if SIZE_MAX==UINT64_MAX
		10, 10, 10, 11, 11, 11,
		12, 12, 12, 12, 13, 13, 13, 14,
		14, 14, 15, 15, 15, 15, 16, 16,
		16, 17, 17, 17, 18, 18, 18, 18, 
#endif
         0
	};
	size_t const powers_of_10[] = {
		1UL, 10UL, 100UL, 1000UL, 10000UL,
		100000UL, 1000000UL, 10000000UL, 100000000UL, 1000000000UL
#if SIZE_MAX==UINT64_MAX
		,
		10000000000UL, 100000000000UL, 1000000000000UL, 10000000000000UL, 100000000000000UL,
		1000000000000000UL, 10000000000000000UL, 100000000000000000UL,
		1000000000000000000UL, 10000000000000000000UL
#endif
	};
	size_t const log2_of_x = int_log2(x);
	size_t const power_idx = log10_tab[log2_of_x];
	size_t const next_pow  = powers_of_10[power_idx + 1];
	return power_idx + (0 < next_pow && next_pow <= x);
}

static inline size_t base_10_digits(size_t const x) {
	return int_log10(x) + 1;
}

static inline size_t base_10_num_chars_uint(size_t const x) {
	return base_10_digits(x) + 1;
}
static inline size_t base_10_num_chars_int(ssize_t const x) {
#ifdef C11
	ssize_t const y = _Generic(x, int: abs(x), long: labs(x), long long: llabs(x));
#else
	ssize_t const y = 
#	if SIZE_MAX==UINT_MAX
		abs(x)
#	elif SIZE_MAX==ULONG_MAX
		labs(x)
#	elif SIZE_MAX==ULLONG_MAX
		llabs(x)
#	endif
	;
#endif
	return base_10_digits(y) + 2; /// 2 for the negative sign symbol.
}


/**
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

	size_t const base = 3;
    size_t log_table[MAX_LOG_TABLE_SIZE]={0}, *powers=NULL;
    size_t const num_powers = make_int_log_tables(base, &log_table, &powers);
    printf("num powers: %zu\n", num_powers);
    
    puts("");
    for( size_t i=0; i < MAX_LOG_TABLE_SIZE; i++ ) {
        printf("log%zu table[%zu]: %zu\n", base, i, log_table[i]);
    }
    
    puts("");
    for( size_t i=0; i < num_powers; i++ ) {
        printf("powers of %zu table[%zu]: %zu\n", base, i, powers[i]);
    }
    
    puts("");
    //for( size_t i=0; i < num_powers; i++ ) {
        size_t const x = -1UL;
        size_t const res = int_log(x, &log_table, &powers[0]);
        printf("log%zu(%zu) == %zu\n", base, x, res);
    //}
    
    free(powers); powers = NULL;
 */
enum {
	MAX_LOG_TABLE_SIZE = (sizeof(size_t) * CHAR_BIT) + 1
};

static inline NO_NULL size_t int_log(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
#ifdef __cplusplus
	size_t const *const powers_of_N
#else
	size_t const powers_of_N[const static 1]
#endif
) {
	size_t const log2_of_x = int_log2(x);
	size_t const power_idx = (*logN_table)[log2_of_x];
	size_t const next_pow  = powers_of_N[power_idx + 1];
	return power_idx + (0 < next_pow && next_pow <= x);
}

static inline NO_NULL size_t make_int_log_tables(size_t const base, size_t (*const logN_table)[MAX_LOG_TABLE_SIZE], size_t **const restrict powers_of_N) {
	if( base < 3 ) {
		return 0;
	}
	/// floor( log ~0 / log base ) === highest power of the base `size_t` or other int types can reach.
	size_t const num_exp = ( size_t )(log(SIZE_MAX) / log(base)) + 1;
	size_t *powers = calloc(num_exp + 1, sizeof *powers);
	if( powers==NULL ) {
		return 0;
	}
	
	/// Generate our powers of the base.
	powers[0] = 1;
	for( size_t i=1; i < num_exp; i++ ) {
		size_t const prev_pow = powers[i-1];
		powers[i] = prev_pow * base;
	}
	*powers_of_N = powers;
	
	/// generate log of N table.
	for( size_t i=0; i < MAX_LOG_TABLE_SIZE; i++ ) {
		(*logN_table)[i] = floor( log(1UL << i) / log(base) );
	}
	return num_exp;
}


static inline size_t base_N_digits(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
#ifdef __cplusplus
	size_t const *const powers_of_N
#else
	size_t const powers_of_N[const static 1]
#endif
) {
	return int_log(x, logN_table, powers_of_N) + 1;
}

static inline size_t base_N_num_chars_uint(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
#ifdef __cplusplus
	size_t const *const powers_of_N
#else
	size_t const powers_of_N[const static 1]
#endif
) {
	return base_N_digits(x, logN_table, powers_of_N) + 1;
}

static inline size_t base_N_num_chars_int(
	ssize_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
#ifdef __cplusplus
	size_t const *const powers_of_N
#else
	size_t const powers_of_N[const static 1]
#endif
) {
#ifdef C11
	ssize_t const y = _Generic( x, int: abs(x), long: labs(x), long long: llabs(x) );
#else
	ssize_t const y = 
#	if SIZE_MAX==UINT_MAX
		abs(x)
#	elif SIZE_MAX==ULONG_MAX
		labs(x)
#	elif SIZE_MAX==ULLONG_MAX
		llabs(x)
#	endif
	;
#endif
	return base_N_digits(y, logN_table, powers_of_N) + 2; /// 2 for the negative sign symbol.
}



#endif /** HARBOL_COMMON_INCLUDES_INCLUDED */
