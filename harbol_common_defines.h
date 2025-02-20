#ifndef HARBOL_COMMON_DEFINES_INCLUDED
#	define HARBOL_COMMON_DEFINES_INCLUDED

/** Check if Windows */
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#	ifndef OS_WINDOWS
#		define OS_WINDOWS 1
#	endif

/** Check if Linux/UNIX & FreeBSD */
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__linux__) || defined(linux) || defined(__linux) || defined(__freeBSD__)
#	ifndef OS_LINUX_UNIX
#		define OS_LINUX_UNIX 1
#	endif

/** Check if Android */
#elif defined(__ANDROID__)
#	ifndef OS_ANDROID
#		define OS_ANDROID 1
#	endif

/** Check if Solaris/SunOS */
#elif defined(sun) || defined(__sun)
#	if defined(__SVR4) || defined(__svr4__)
#		ifndef OS_SOLARIS
#			define OS_SOLARIS 1
#		endif
#	else
#		ifndef OS_SUNOS
#			define OS_SUNOS 1
#		endif
#	endif

/** Check if Macintosh/MacOS/iOS */
#elif defined(macintosh) || defined(Macintosh) || defined(__APPLE__)
#	include "TargetConditionals.h"
#	if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
#		ifndef OS_IPHONE_SIM
#			define OS_IPHONE_SIM 1
#		endif
#		ifndef OS_IPHONE
#			define OS_IPHONE 1
#		endif
#	elif TARGET_OS_IPHONE
#		ifndef OS_IPHONE
#			define OS_IPHONE 1
#		endif
#	else
#		ifndef OS_MAC
#			define OS_MAC 1
#		endif
#	endif

#endif /** end OS checks */

/** check what compiler we got */
#if defined(__clang__)
#	ifndef COMPILER_CLANG
#		define COMPILER_CLANG
#	endif
#elif defined(__GNUC__) || defined(__GNUG__)
#	ifndef COMPILER_GCC
#		define COMPILER_GCC
#	endif
#elif defined(_MSC_VER)
#	ifndef COMPILER_MSVC
#		define COMPILER_MSVC
#	endif
#elif defined(__INTEL_COMPILER)
#	ifndef COMPILER_INTEL
#		define COMPILER_INTEL
#	endif
#endif /** end compiler check macros */

/** check arch platform. */
#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_AMD64)
#	ifndef PLATFORM_AMD64
#		define PLATFORM_AMD64
#	endif
#	ifndef PLATFORM_x64
#		define PLATFORM_x64
#	endif
#	ifndef PLATFORM_x8664
#		define PLATFORM_x8664
#	endif
#	ifndef PLATFORM_x86_64
#		define PLATFORM_x86_64
#	endif
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__I86__) || defined(__386)
#	ifndef PLATFORM_IA32
#		define PLATFORM_IA32
#	endif
#	ifndef PLATFORM_x86
#		define PLATFORM_x86
#	endif
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) || defined(__itanium__)
#	ifndef PLATFORM_IA64
#		define PLATFORM_IA64
#	endif
#	ifndef PLATFORM_ITANIUM
#		define PLATFORM_ITANIUM
#	endif
#elif defined(__arm__) || defined(_ARM) || defined(_M_ARM) || defined(__arm)
#	ifndef PLATFORM_ARM32
#		define PLATFORM_ARM32
#	endif
#elif defined(__aarch64__)
#	ifndef PLATFORM_ARM64
#		define PLATFORM_ARM64
#	endif
#elif defined(__riscv) || defined(_riscv) || defined(__riscv__)
#	ifndef PLATFORM_RISCV
#		define PLATFORM_RISCV
#	endif
#	if defined(__riscv_xlen) && __riscv_xlen==32
#		ifndef PLATFORM_RISCV32
#			define PLATFORM_RISCV32
#		endif
#	elif defined(__riscv_xlen) && __riscv_xlen==64
#		ifndef PLATFORM_RISCV64
#			define PLATFORM_RISCV64
#		endif
#	endif
#endif /** end platform arch defines. */

/** set up the C standard macros! */
#ifdef __STDC__
#	ifndef C89
#		define C89
#	endif
#	ifdef __STDC_VERSION__
#		ifndef C90
#			define C90
#		endif
#		if (__STDC_VERSION__ >= 199409L)
#			ifndef C94
#				define C94
#			endif
#		endif
#		if (__STDC_VERSION__ >= 199901L)
#			ifndef C99
#				define C99
#			endif
#		endif
#		if (__STDC_VERSION__ >= 201112L)
#			ifndef C11
#				define C11
#			endif
#		endif
#		if (__STDC_VERSION__ >= 201710L)
#			ifndef C18
#				define C18
#			endif
#		endif
#		if (__STDC_VERSION__ >= 202311L)
#			ifndef C23
#				define C23
#			endif
#		endif
#	endif
#endif

#ifdef __cplusplus
#	if( __cplusplus >= 199711L )
#		ifndef CPP03
#			define CPP03
#		endif
#	endif
#	if( __cplusplus >= 201103L )
#		ifndef CPP11
#			define CPP11
#		endif
#	endif
#	if( __cplusplus >= 201402L )
#		ifndef CPP14
#			define CPP14
#		endif
#	endif
#	if( __cplusplus >= 201703L )
#		ifndef CPP17
#			define CPP17
#		endif
#	endif
#	if( __cplusplus >= 202002L )
#		ifndef CPP20
#			define CPP20
#		endif
#	endif
#	if( __cplusplus >= 202302L )
#		ifndef CPP23
#			define CPP23
#		endif
#	endif
#	if( __cplusplus > 202302L )
#		ifndef CPP26
#			define CPP26
#		endif
#	endif
#endif


/** setup RAII destructor macro if possibru to mark functions as cleaner-uppers. */
#ifndef RAII_DTOR
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define RAII_DTOR(func) __attribute__ ((cleanup((func))))
#	else
#		define RAII_DTOR(func)
#	endif
#endif

/** setup macro to mark a param as "cannot or can never be NULL". */
#ifndef NEVER_NULL
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define NEVER_NULL(...) __attribute__( (nonnull(__VA_ARGS__)) )
#	else
#		define NEVER_NULL(...)
#	endif
#endif

/** setup macro that declares all params to never be NULL. */
#ifndef NO_NULL
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define NO_NULL __attribute__((nonnull))
#	else
#		define NO_NULL
#	endif
#endif

/** setup macro to mark a function as never returning a null pointer. */
#ifndef NONNULL_RET
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define NONNULL_RET __attribute__((returns_nonnull))
#	else
#		define NONNULL_RET
#	endif
#endif

/** setup macro that marks a function as deprecated. */
#ifndef DEPRECATED
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define DEPRECATED(func) func __attribute__ ((deprecated))
#	elif defined(COMPILER_MSVC)
#		define DEPRECATED(func) __declspec(deprecated) func
#	else
#		define DEPRECATED(func)
#	endif
#endif

/** setup macro that defines a var that it may be aliased by other data. */
#ifndef PTR_ALIAS
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define PTR_ALIAS __attribute__ ((__may_alias__))
#	else
#		define PTR_ALIAS
#	endif
#endif

/** setup macro that marks a function as having hidden visibility. */
#ifndef VIS_HIDDEN
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define VIS_HIDDEN __attribute__ ((visibility ("hidden")))
#	else
#		define VIS_HIDDEN
#	endif
#endif

/** setup macro that marks a function as having internal visibility. */
#ifndef VIS_INTERNAL
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define VIS_INTERNAL __attribute__ ((visibility ("internal")))
#	else
#		define VIS_INTERNAL
#	endif
#endif

/** setup macro that marks a function as having protected visibility. */
#ifndef VIS_PROTECTED
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define VIS_PROTECTED __attribute__ ((visibility ("protected")))
#	else
#		define VIS_PROTECTED
#	endif
#endif

/** setup macro that gives a warning if a function's result is unused. */
#ifndef WARN_UNUSED_RESULT
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define WARN_UNUSED_RESULT __attribute__ ((warn_unused_result))
#	else
#		define WARN_UNUSED_RESULT
#	endif
#endif

/** setup macro to mark a function or variable as unused and ignore unused warnings. */
#ifndef UNUSED
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define UNUSED __attribute__ ((unused))
#		define HARBOL_UNUSED(a)    ((void)(a))
#	else
#		define UNUSED
#		define HARBOL_UNUSED(a)    ((void)(a))
#	endif
#endif

/** setup macro to mark a function as a hot spot, thus requiring aggressive optimizations. */
#ifndef HOT
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define HOT __attribute__ ((hot))
#	else
#		define HOT
#	endif
#endif

/** setup macro to make vector types. Argument must be power of 2.
 * Example:
	typedef __attribute__ ((vector_size (32))) int int_vec32_t; which makes int_vec32_t as 32-bytes.
 * 
 * All the basic integer types can be used as base types, both as signed and as unsigned: char, short, int, long, long long. In addition, float and double can be used to build floating-point vector types. 
 * +, -, *, /, unary minus, ^, |, &, ~, %. are the only operators allowed.
 * 
 * Bigger Example:
	typedef int v4si SIMD_VEC(16);
	v4si a = {1,2,3,4};
	v4si b = {3,2,1,4};
	v4si c;
	
	c = a >  b;     /// The result would be {0, 0,-1, 0}
	c = a == b;     /// The result would be {0,-1, 0,-1}
 */
#ifndef SIMD_VEC
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define SIMD_VEC(bytes) __attribute__ ((vector_size ((bytes))))
#	else
#		define SIMD_VEC(bytes)
#	endif
#endif


#ifdef HARBOL_DLL
#	ifndef HARBOL_LIB 
#		define HARBOL_EXPORT __declspec(dllimport)
#	else
#		define HARBOL_EXPORT __declspec(dllexport)
#	endif
#else
#	define HARBOL_EXPORT
#endif

#ifdef __cplusplus
#	ifdef OS_WINDOWS
#		ifndef restrict
#			define restrict __restrict
#		endif
#	else
#		ifndef restrict
#			define restrict __restrict__
#		endif
#	endif
#endif


#ifdef COMPILER_MSVC
#	ifndef inline
#		define inline    __inline
#	endif
#elif !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
#	ifndef inline
#		define inline    __inline__
#	endif
#endif


#ifdef C11
#	ifndef IS_VAR_OF_TYPE
#		define IS_VAR_OF_TYPE(n, T)     _Generic((n), T:true, default:false)
#	endif

#	ifndef IS_SIGNED_INT_TYPE
#		define IS_SIGNED_INT_TYPE(n)    _Generic((n),                 \
                                            _Bool:              true, \
                                            char:               true, \
                                            signed char:        true, \
                                            int8_t:             true, \
                                            short:              true, \
                                            signed short:       true, \
                                            int16_t:            true, \
                                            int:                true, \
                                            signed int:         true, \
                                            signed:             true, \
                                            int32_t:            true, \
                                            long:               true, \
                                            signed long:        true, \
                                            long long:          true, \
                                            signed long long:   true, \
                                            ptrdiff_t:          true, \
                                            int64_t:            true, \
                                            intmax_t:           true, \
                                            default:            false \
                                        )
#	endif

#	ifndef IS_UNSIGNED_INT_TYPE
#		define IS_UNSIGNED_INT_TYPE(n)  _Generic((n),                 \
                                            _Bool:              true, \
                                            unsigned char:      true, \
                                            uint8_t:            true, \
                                            unsigned short:     true, \
                                            uint16_t:           true, \
                                            unsigned int:       true, \
                                            unsigned:           true, \
                                            uint32_t:           true, \
                                            unsigned long:      true, \
                                            size_t:             true, \
                                            unsigned long long: true, \
                                            uint64_t:           true, \
                                            uintmax_t:          true, \
                                            default:            false \
                                        )
#	endif

#	ifndef IS_FLOAT_TYPE
#		define IS_FLOAT_TYPE(n)         _Generic((n), float: true, double: true, long double: true, default:false)
#	endif

#	ifndef IS_NUMERIC_TYPE
#		define IS_NUMERIC_TYPE(n)       (IS_FLOAT_TYPE(n) || IS_UNSIGNED_INT_TYPE(n) || IS_SIGNED_INT_TYPE(n))
#	endif

#	ifndef IS_SIGNED_TYPE
#		define IS_SIGNED_TYPE(n)        (IS_FLOAT_TYPE(n) || IS_SIGNED_INT_TYPE(n))
#	endif

#	ifndef TYPE_TO_CSTR
#		define TYPE_TO_CSTR(n)          _Generic((n),                                 \
                                            _Bool:              "bool",               \
                                            char:               "char",               \
                                            signed char:        "signed char",        \
                                            unsigned char:      "unsigned char",      \
                                            int8_t:             "int8_t",             \
                                            uint8_t:            "uint8_t",            \
                                            short:              "short",              \
                                            signed short:       "signed short",       \
                                            unsigned short:     "unsigned short",     \
                                            int16_t:            "int16_t",            \
                                            uint16_t:           "uint16_t",           \
                                            int:                "int",                \
                                            signed int:         "signed int",         \
                                            unsigned int:       "unsigned int",       \
                                            int32_t:            "int32_t",            \
                                            uint32_t:           "uint32_t",           \
                                            long:               "long",               \
                                            signed long:        "signed long",        \
                                            unsigned long:      "unsigned long",      \
                                            size_t:             "size_t",             \
                                            ssize_t:            "ssize_t",            \
                                            long long:          "long long",          \
                                            signed long long:   "signed long long",   \
                                            unsigned long long: "unsigned long long", \
                                            ptrdiff_t:          "ptrdiff_t",          \
                                            int64_t:            "int64_t",            \
                                            uint64_t:           "uint64_t",           \
                                            intmax_t:           "intmax_t",           \
                                            uintmax_t:          "uintmax_t",          \
                                            intptr_t:           "intptr_t",           \
                                            uintptr_t:          "uintptr_t",          \
                                            default:            "unknown"             \
                                        )
#	endif

#	ifndef TYPE_TO_CSTR
#		define TYPE_TO_CSTR(n)          _Generic((n),                                 \
                                            _Bool:              "bool",               \
                                            char:               "char",               \
                                            signed char:        "signed char",        \
                                            unsigned char:      "unsigned char",      \
                                            int8_t:             "int8_t",             \
                                            uint8_t:            "uint8_t",            \
                                            short:              "short",              \
                                            signed short:       "signed short",       \
                                            unsigned short:     "unsigned short",     \
                                            int16_t:            "int16_t",            \
                                            uint16_t:           "uint16_t",           \
                                            int:                "int",                \
                                            signed int:         "signed int",         \
                                            unsigned int:       "unsigned int",       \
                                            int32_t:            "int32_t",            \
                                            uint32_t:           "uint32_t",           \
                                            long:               "long",               \
                                            signed long:        "signed long",        \
                                            unsigned long:      "unsigned long",      \
                                            size_t:             "size_t",             \
                                            ssize_t:            "ssize_t",            \
                                            long long:          "long long",          \
                                            signed long long:   "signed long long",   \
                                            unsigned long long: "unsigned long long", \
                                            ptrdiff_t:          "ptrdiff_t",          \
                                            int64_t:            "int64_t",            \
                                            uint64_t:           "uint64_t",           \
                                            intmax_t:           "intmax_t",           \
                                            uintmax_t:          "uintmax_t",          \
                                            intptr_t:           "intptr_t",           \
                                            uintptr_t:          "uintptr_t",          \
                                            float:              "float",              \
                                            double:             "double",             \
                                            long double:        "long double",        \
                                            float32_t:          "float32_t",          \
                                            float64_t:          "float64_t",          \
                                            floatptr_t:         "floatptr_t",         \
                                            floatmax_t:         "floatmax_t",         \
                                            default:            "unknown"             \
                                        )
#	endif

#	ifndef TYPE_FMT_SPEC
#		define TYPE_FMT_SPEC(n)         _Generic((n),                           \
                                            _Bool:              "%u",           \
                                            char:               "%c",           \
                                            signed char:        "%hhi",         \
                                            unsigned char:      "%hhu",         \
                                            int8_t:             "%" PRIi8 "",   \
                                            uint8_t:            "%" PRIu8 "",   \
                                            short:              "%hi",          \
                                            signed short:       "%hi",          \
                                            unsigned short:     "%hu",          \
                                            int16_t:            "%" PRIi16 "",  \
                                            uint16_t:           "%" PRIu16 "",  \
                                            int:                "%i",           \
                                            signed int:         "%i",           \
                                            unsigned int:       "%u",           \
                                            int32_t:            "%" PRIi32 "",  \
                                            uint32_t:           "%" PRIu32 "",  \
                                            long:               "%li",          \
                                            signed long:        "%li",          \
                                            unsigned long:      "%lu",          \
                                            size_t:             "%zu",          \
                                            ssize_t:            "%zi",          \
                                            long long:          "%lli",         \
                                            signed long long:   "%lli",         \
                                            unsigned long long: "%llu",         \
                                            ptrdiff_t:          "%ti",          \
                                            int64_t:            "%" PRIi64 "",  \
                                            uint64_t:           "%" PRIu64 "",  \
                                            intmax_t:           "%" PRIiMAX "", \
                                            uintmax_t:          "%" PRIuMAX "", \
                                            intptr_t:           "%" PRIiPTR "", \
                                            uintptr_t:          "%" PRIuPTR "", \
                                            float:              "%f",           \
                                            double:             "%f",           \
                                            long double:        "%Lf",          \
                                            float32_t:          "%" PRIf32 "",  \
                                            float64_t:          "%" PRIf64 "",  \
                                            floatptr_t:         "%" PRIfPTR "", \
                                            floatmax_t:         "%" PRIfMAX "", \
                                            default:            "%%"            \
                                        )
#	endif

#	ifndef PRINT_TYPE
#		define PRINT_TYPE(stream, n)        fprintf((stream), TYPE_TO_CSTR(n) "\n")
#	endif

#	ifndef PRINT_VAL
#		define PRINT_VAL(stream, n)         fprintf((stream), TYPE_FMT_SPEC(n) "\n", (n))
#	endif

#	ifndef PRINT_TYPE_VAL
#		define PRINT_TYPE_VAL(stream, n)    fprintf((stream), TYPE_TO_CSTR(n) ": " TYPE_FMT_SPEC(n) "\n", (n))
#	endif
#endif /** ifdef C11 */


#ifndef TAIL_CALL
#	if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#		define TAIL_CALL    __attribute__ ((musttail))
#	else
#		define TAIL_CALL
#	endif
#endif

#ifndef REG_CALL
#	if defined(PLATFORM_AMD64) && (defined(COMPILER_CLANG) || defined(COMPILER_INTEL))
#		define REG_CALL    __attribute__ ((regcall))
#	else
#		define REG_CALL
#	endif
#endif

#ifndef LVAL_PTR
#	define LVAL_PTR(T, expr)    (&(T){ expr })
#endif

#ifndef TYPEOF
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC) || defined(COMPILER_MSVC)
#		define TYPEOF(a)    __typeof__ (a)
#	else
#		define TYPEOF(a)
#	endif
#endif

#ifndef AUTO_TYPE
#	if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
#		define AUTO_TYPE    __auto_type
#	else
#		define AUTO_TYPE
#	endif
#endif


#ifndef ARRAY_LEN
#	define ARRAY_LEN(array)    (sizeof((array)) / sizeof((array)[0]))
#endif
/**
 * Dark C Magic to corrupt the youth:
 * 
 * T array[...] = ...;
 * for( size_t i=0; i < (1[&array] - array); i++ )
 */


/**
 * Keyword Reducers.
 */

#ifndef HELPER_FUNC
#	define HELPER_FUNC          static inline
#endif

/// Keyword Helpers for C and C++ compatibility.
#ifdef __cplusplus
#	ifndef ARRAY_PARAM_CONST
#		define ARRAY_PARAM_CONST(name)     *const name
#	endif
	
#	ifndef ARRAY_PARAM_REF
#		define ARRAY_PARAM_REF(name)       *const restrict name
#	endif
	
#	ifndef ARRAY_PARAM_LIM
#		define ARRAY_PARAM_LIM(name, len)  ARRAY_PARAM_CONST(name)
#	endif
	
#	ifndef ARRAY_PARAM_FULL
#		define ARRAY_PARAM_FULL(name, len) ARRAY_PARAM_REF(name)
#	endif
	
#	ifndef C_PTR_CAST
#		define C_PTR_CAST(a, expr)         reinterpret_cast< decltype(a) >( (expr) );
#	endif
	
/// This part for compiling C.
#else
#	ifndef ARRAY_PARAM_CONST
#		define ARRAY_PARAM_CONST(name)     name[const]
#	endif

#	ifndef ARRAY_PARAM_REF
#		define ARRAY_PARAM_REF(name)       name[const restrict]
#	endif

#	ifndef ARRAY_PARAM_LIM
#		define ARRAY_PARAM_LIM(name, len)  name[const static (len)]
#	endif

#	ifndef ARRAY_PARAM_FULL
#		define ARRAY_PARAM_FULL(name, len) name[const restrict static (len)]
#	endif
	
#	ifndef C_PTR_CAST
#		define C_PTR_CAST(a, expr)        ( void* )( (expr) );
#	endif

#endif

#ifdef HARBOL_DEBUG_PRINTING
#	ifndef HARBOL_DBG_PRINT
#		define HARBOL_DBG_PRINT(str_lit, ...)    printf("%s :: " str_lit "\n" , __func__, __VA_ARGS__)
#	endif
#else
#	ifndef HARBOL_DBG_PRINT
#		define HARBOL_DBG_PRINT(str_lit, ...)
#	endif
#endif

#ifndef HARBOL_SUBCSTR_FMT
#	define HARBOL_SUBCSTR_FMT    "%.*s"
#endif

#endif /** HARBOL_COMMON_DEFINES_INCLUDED */