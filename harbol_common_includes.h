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

#ifdef OS_WINDOWS
#	include <intrin.h>
#endif

#ifdef __cplusplus
#	include <type_traits>
#endif

/** placing this here so we can get this after including inttypes.h */
#if defined(SIZE_MAX)
#	if defined(UINT32_MAX) && SIZE_MAX==UINT32_MAX
#		ifdef HARBOL32
#			undef HARBOL32
#		endif
#		define HARBOL32
#	endif
#	if defined(UINT64_MAX) && SIZE_MAX==UINT64_MAX
#		ifdef HARBOL64
#			undef HARBOL64
#		endif
#		define HARBOL64
#	endif
#endif


#include "harbol_types.h"
#include "harbol_bits.h"
#include "harbol_hashers.h"
#include "harbol_allocs.h"
#include "harbol_fileutil.h"
#include "harbol_numbounds.h"
#include "harbol_bufops.h"
#include "harbol_int_logarithm.h"
#include "harbol_intrusive_lists.h"

#endif /** HARBOL_COMMON_INCLUDES_INCLUDED */