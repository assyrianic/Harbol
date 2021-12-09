#ifndef HARBOL_INCLUDED
#	define HARBOL_INCLUDED
#	define HARBOL_VERSION_MAJOR    3
#	define HARBOL_VERSION_MINOR    0
#	define HARBOL_VERSION_PATCH    0
#	define HARBOL_VERSION_PHASE    "beta"
#	define STR_HELPER(x)           #x
#	define STR(x)    STR_HELPER(x)
#	define HARBOL_VERSION_STRING \
			STR(HARBOL_VERSION_MAJOR) "." STR(HARBOL_VERSION_MINOR) "." STR(HARBOL_VERSION_PATCH) " " HARBOL_VERSION_PHASE


#ifdef __cplusplus
extern "C" {
#endif


#include "harbol_common_defines.h"

#ifndef C99
#	error "Harbol requires a C99 compliant compiler with at least stdbool.h, inttypes.h, compound literals, and designated initializers."
#endif

/// General-Purpose Free List-based Memory Pool
#include "allocators/mempool/mempool.h"

/// Fast & Efficient Object Pool
#include "allocators/objpool/objpool.h"

/// Simple & Efficient Region Allocator Pool
#include "allocators/region/region.h"

/// C++ Style String Type
#include "str/str.h"

/// Low-Level Dynamic/Static Array
#include "array/array.h"

/// Tuple Type
#include "tuple/tuple.h"

/// Byte Buffer
#include "bytebuffer/bytebuffer.h"

/// Ordered Hash Table
#include "map/map.h"

/// n-Ary Tree
#include "tree/tree.h"

/// Variant Type
#include "variant/variant.h"

/// JSON-like Config Parser
#include "cfg/cfg.h"

/// Shared Library Plugins
#include "plugins/plugins.h"

/// Vector-based Double Ended Queue
#include "deque/deque.h"

/// Lexing Tools
#include "lex/lex.h"

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_INCLUDED */