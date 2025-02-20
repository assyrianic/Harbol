#ifndef HARBOL_BITS_INCLUDED
#	define HARBOL_BITS_INCLUDED


/**
 * ALL header funcs concerning bitwise or bit tricks go here.
 */

HELPER_FUNC size_t harbol_align_size(size_t const size, size_t const align) {
	return (size + (align - 1UL)) & ~(align - 1UL);
}

HELPER_FUNC bool is_size_aligned(size_t const size, size_t const align) {
	return (size % align)==0;
}

HELPER_FUNC size_t harbol_pad_size(size_t const size, size_t const align) {
	return (align - (size & (align - 1UL))) & (align - 1UL);
}

HELPER_FUNC size_t harbol_size_bits(size_t const bytes) {
	return bytes * CHAR_BIT;
}

HELPER_FUNC size_t harbol_bit_index(size_t const idx) {
	return (( size_t )(1UL)) << idx;
}

HELPER_FUNC size_t harbol_signbit(size_t const bytes) {
	return harbol_bit_index(harbol_size_bits(bytes) - 1UL);
}

HELPER_FUNC size_t harbol_bit_mask(size_t const idx) {
	return harbol_bit_index(idx) - 1UL;
}

HELPER_FUNC size_t harbol_bit_prev(size_t const idx) {
	return harbol_bit_index(idx - 1UL);
}

HELPER_FUNC size_t harbol_bytemask(size_t const bytes) {
	return harbol_signbit(bytes) | (harbol_signbit(bytes) - 1UL);
}

HELPER_FUNC size_t bitwise_ceil(size_t x) {
	x |= x >> 1UL;
	x |= x >> 2UL;
	x |= x >> 4UL;
	x |= x >> 8UL;
#if SIZE_MAX>=UINT32_MAX
	x |= x >> 16UL;
#endif
#if SIZE_MAX>=UINT64_MAX
	x |= x >> 32UL;
#endif
	return x;
}

HELPER_FUNC size_t next_pow_of_2(size_t const x) {
	return bitwise_ceil(x) + 1UL;
}

#ifdef HARBOL_BITWISE_CEIL_SCALABLE
#	undef HARBOL_BITWISE_CEIL_SCALABLE
#endif
#define HARBOL_BITWISE_CEIL_SCALABLE(x)    do { \
                                               size_t const highest_bit_order = harbol_size_bits(sizeof (x)) >> 1UL; \
                                               for( size_t i=1UL; i <= highest_bit_order; i <<= 1UL ) { \
                                                   x |= (x) >> i; \
                                               } \
                                           }while(0)


/// this is for sign-extending smaller, unsigned ints to larger signed ints.
HELPER_FUNC ssize_t harbol_sign_ext_bits(size_t const a, size_t const b) {
	return ( (a & harbol_bit_mask(b)) ^ harbol_bit_prev(b) ) - harbol_bit_prev(b);
}

#ifdef HARBOL_SIGN_EXT
#	undef HARBOL_SIGN_EXT
#endif
#define HARBOL_SIGN_EXT(a)    harbol_sign_ext_bits( (a), harbol_size_bits(sizeof(a)) )


HELPER_FUNC uintptr_t get_ptr_alignment(void const *const ptr, size_t const bytes) {
	return ( uintptr_t )(ptr) & (bytes - 1UL);
}

HELPER_FUNC bool is_ptr_aligned(void const *const ptr, size_t const bytes) {
	return get_ptr_alignment(ptr, bytes)==0;
}

HELPER_FUNC size_t harbol_popcount(size_t const x) {
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	return __builtin_popcountl(x);
#elif defined(COMPILER_MSVC)
#	if SIZE_MAX==UINT32_MAX
		return __popcnt(x);
#	elif SIZE_MAX==UINT64_MAX
		return __popcnt64(x);
#	endif
#else
	size_t count = 0;
	for( size_t i = x; i > 0; count++ ) {
		i &= (i - 1);
	}
	return count;
#endif
}

HELPER_FUNC void harbol_print_bits(FILE *const stream, size_t const n, size_t const nbits, bool const newline) {
#ifdef C23
	HARBOL_UNUSED(nbits);
	fprintf(stream, newline? "%b\n" : "%b", n);
#else
	char bits_cstr[(sizeof n * CHAR_BIT) + 2] = {0};
	size_t j = 0;
	for( size_t i = nbits - 1; i <= nbits; i-- ) {
		bits_cstr[j++] = '0' + ((n >> i) & 1);
	}
	if( newline ) {
		bits_cstr[j++] = '\n';
	}
	if( j < sizeof bits_cstr ) {
		fprintf(stream, "%s", bits_cstr);
	}
#endif
}


#endif /** HARBOL_BITS_INCLUDED */