#ifndef HARBOL_HASHERS_INCLUDED
#	define HARBOL_HASHERS_INCLUDED


static inline size_t _SDBM_hash(size_t const curr_key, size_t const curr_hash) {
	return curr_key + (curr_hash << 6U) + (curr_hash << 16U) - curr_hash;
}


/// these are NOT cryptographic hashes.
/// use ONLY FOR HASH TABLE IMPLEMENTATIONS.
/// The implementation is a [largely unmodified] SDBM algorithm
HELPER_FUNC size_t string_hash(char const ARRAY_PARAM_LIM(key, 1), size_t const seed) {
	size_t h = seed;
	for( size_t i=0; key[i] != 0; i++ ) {
		h = _SDBM_hash(key[i], h);
	}
	return h;
}

HELPER_FUNC size_t array_hash(uint8_t const ARRAY_PARAM_LIM(key, 1), size_t const len, size_t const seed) {
	size_t h = seed;
	for( size_t i=0; i < len; i++ ) {
		h = _SDBM_hash(key[i], h);
	}
	return h;
}

HELPER_FUNC size_t int_hash(size_t const i, size_t const seed) {
	size_t h = seed;
	for( size_t n=0; n < harbol_size_bits(sizeof n); n += CHAR_BIT ) {
		h = _SDBM_hash((i >> n) & 0xFFul, h);
	}
	return h;
}

HELPER_FUNC size_t float_hash(floatptr_t const a, size_t const seed) {
	union {
		floatptr_t const f;
		size_t     const s;
	} const c = {a};
	return int_hash(c.s, seed);
}

HELPER_FUNC NO_NULL size_t ptr_hash(void const *const p, size_t const seed) {
	union {
		void const *const p;
		uintptr_t const   u;
	} const c = { p };
	return (c.u >> 4ul) | ((c.u << (8ul * sizeof c.p - 4ul)) + seed);
}

#ifdef C11
#	ifdef harbol_hash
#		undef harbol_hash
#	endif
#	define harbol_hash(h, s) _Generic((h)+0,                \
                                bool        : int_hash,     \
                                int8_t      : int_hash,     \
                                uint8_t     : int_hash,     \
                                int16_t     : int_hash,     \
                                uint16_t    : int_hash,     \
                                uint32_t    : int_hash,     \
                                int32_t     : int_hash,     \
                                size_t      : int_hash,     \
                                ssize_t     : int_hash,     \
                                int64_t     : int_hash,     \
                                uint64_t    : int_hash,     \
                                float32_t   : float_hash,   \
                                float64_t   : float_hash,   \
                                floatptr_t  : float_hash,   \
                                char*       : string_hash,  \
                                char const* : string_hash,  \
                                default     : ptr_hash      \
                            )((h), (s))
#endif

#endif /** HARBOL_HASHERS_INCLUDED */