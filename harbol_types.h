#ifndef HARBOL_TYPES_INCLUDED
#	define HARBOL_TYPES_INCLUDED


#if !defined(__cplusplus) && !defined(C23)
#	ifdef nullptr
#		undef nullptr
#	endif
#	define nullptr    (( void* )(0))
#endif

/// check if '[u]intptr_t' is defined since it's optional for C++.
#if defined(__cplusplus) && !defined(__intptr_t_defined)
#	ifndef INTPTR_MIN
#		#define INTPTR_MIN     INTMAX_MIN
#	endif
#	ifndef INTPTR_MAX
#		#define INTPTR_MAX     INTMAX_MAX
#	endif
#	ifndef UINTPTR_MIN
#		#define UINTPTR_MIN    UINTMAX_MIN
#	endif
#	ifndef UINTPTR_MAX
#		#define UINTPTR_MAX    UINTMAX_MAX
#	endif
#	define __intptr_t_defined
	typedef signed   long long     intptr_t;
	typedef unsigned long long    uintptr_t;
	static_assert(sizeof( intptr_t)>=sizeof(void*), "Unexpected ` intptr_t` size");
	static_assert(sizeof(uintptr_t)>=sizeof(void*), "Unexpected `uintptr_t` size");
#endif


#ifndef intnullptr
#	if defined(__cplusplus) || defined(C23)
#		define intnullptr    ( uintptr_t )(( void* )(nullptr))
#	else
#		define intnullptr    ( uintptr_t )(nullptr)
#	endif
#endif

/** types as defined by Harbol. */
#ifndef __ssize_t_defined
#	ifdef SSIZE_MIN
#		undef SSIZE_MIN
#	endif
#	ifdef SSIZE_MAX
#		undef SSIZE_MAX
#	endif
#	if SIZE_MAX==UINT_MAX
		typedef int ssize_t;        /** common 32 bit case */
#		define SSIZE_MIN  INT_MIN
#		define SSIZE_MAX  INT_MAX
#		define PRIiSIZE   "i"
#		define SCNiSIZE   "i"
#		define strtoisize  strtol
#	elif SIZE_MAX==ULONG_MAX
		typedef long ssize_t;       /** linux 64 bits */
#		define SSIZE_MIN  LONG_MIN
#		define SSIZE_MAX  LONG_MAX
#		define PRIiSIZE   "li"
#		define SCNiSIZE   "li"
#		define strtoisize  strtol
#	elif SIZE_MAX==ULLONG_MAX
		typedef long long ssize_t;  /** windows 64 bits */
#		define SSIZE_MIN  LLONG_MIN
#		define SSIZE_MAX  LLONG_MAX
#		define PRIiSIZE   "lli"
#		define SCNiSIZE   "lli"
#		define strtoisize  strtoll
#	endif
#	define __ssize_t_defined
#endif


/** According to C99 standards.
 * There are three floating point types: float, double, and long double.
 * 
 * The type double provides at least as much precision as float, and the type long double provides at least as much precision as double.
 * In summary: float <= double <= long double
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
	_Static_assert(sizeof(floatptr_t)==sizeof(size_t), "Unexpected `floatptr_t` size");
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


#endif /** HARBOL_TYPES_INCLUDED */