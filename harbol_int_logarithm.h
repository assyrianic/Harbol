#ifndef HARBOL_INT_LOGARITHM_INCLUDED
#	define HARBOL_INT_LOGARITHM_INCLUDED


HELPER_FUNC ssize_t int_abs(ssize_t const x) {
	return ( ssize_t )(imaxabs(x));
}

HELPER_FUNC size_t int_log2(size_t const x) {
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	if( x==0 ) {
		return 0;
	}
	return harbol_size_bits(sizeof x) - __builtin_clzl(x) - 1;
#elif defined(SIZE_MAX)
	size_t const de_bruijn_tab[] = {
#	if SIZE_MAX==UINT16_MAX
		 0,  1, 4, 10, 2,  8,  5, 14,
		11,  3, 7,  9, 6, 12, 11, 15,
#	elif SIZE_MAX==UINT32_MAX
		 0,  9,  1, 10, 13, 21,  2, 29,
		11, 14, 16, 18, 22, 25,  3, 30,
		 8, 12, 20, 28, 15, 17, 24,  7,
		19, 27, 23,  6, 26,  5,  4, 31
#	elif SIZE_MAX==UINT64_MAX
		0,  58, 1,  59, 47, 53, 2,  60,
		39, 48, 27, 54, 33, 42, 3,  61,
		51, 37, 40, 49, 18, 28, 20, 55,
		30, 34, 11, 43, 14, 22, 4,  62,
		57, 46, 52, 38, 26, 32, 41, 50,
		36, 17, 19, 29, 10, 13, 21, 56,
		45, 25, 31, 35, 16,  9, 12, 44,
		24, 15,  8, 23,  7,  6,  5, 63
#	endif
	};
	enum {
#	if SIZE_MAX==UINT16_MAX
		DeBruijnLog2Magic = 0x09AFul,
		DeBruijnLog2Shift = 12u, /// 0000 1100
#	elif SIZE_MAX==UINT32_MAX
		DeBruijnLog2Magic = 0x07C4ACDDul,
		DeBruijnLog2Shift = 27u, /// 0001 1011
#	elif SIZE_MAX==UINT64_MAX
		DeBruijnLog2Magic = 0x03F6EAF2CD271461ul,
		DeBruijnLog2Shift = 58u, /// 0011 1010
#	endif
	};
	return de_bruijn_tab[(bitwise_ceil(x) * DeBruijnLog2Magic) >> DeBruijnLog2Shift];
#else
#	error "no valid impl for `int_log2`."
#endif
}

HELPER_FUNC size_t base_2_digits(size_t const x) {
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
	return x > 0? harbol_size_bits(sizeof x) - __builtin_clzl(x) : 0;
#else
	return int_log2(x) + 1;
#endif
}

HELPER_FUNC size_t base_2_num_chars(size_t const x) {
	return base_2_digits(x) + 1;
}


HELPER_FUNC size_t int_log10(size_t const x) {
	size_t const log10_tab[] = {
		 0,  0,  0,  0,  1,  1,  1,  2,
		 2,  2,  3,  3,  3,  3,  4,  4,
		 4,
#if SIZE_MAX==UINT32_MAX
		 5,  5,  5,  6,  6,  6,  6,
		 7,  7,  7,  8,  8,  8,  9,  9,
		 9,  9,
#endif
#if SIZE_MAX==UINT64_MAX
		10, 10, 10, 11, 11, 11,
		12, 12, 12, 12, 13, 13, 13, 14,
		14, 14, 15, 15, 15, 15, 16, 16,
		16, 17, 17, 17, 18, 18, 18, 18, 
#endif
         0
	};
	size_t const powers_of_10[] = {
		1UL, 10UL, 100UL, 1000UL, 10000UL
#if SIZE_MAX==UINT32_MAX
		,
		100000UL, 1000000UL, 10000000UL, 100000000UL, 1000000000UL
#endif
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

HELPER_FUNC size_t base_10_digits(size_t const x) {
	return int_log10(x) + 1;
}

HELPER_FUNC size_t base_10_num_chars_uint(size_t const x) {
	return base_10_digits(x) + 1;
}
HELPER_FUNC size_t base_10_num_chars_int(ssize_t const x) {
	return base_10_digits(int_abs(x)) + 2; /// 2 for the negative sign symbol.
}


/**
	#include <stdio.h>
	#include <inttypes.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <math.h>
	#include <limits.h>
	
	size_t const base = 3;
	size_t log_table[MAX_LOG_TABLE_SIZE]={0}, *powers=nullptr;
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
	
	free(powers); powers = nullptr;
 */
enum {
	MAX_LOG_TABLE_SIZE = (sizeof(size_t) * CHAR_BIT) + 1
};

HELPER_FUNC NO_NULL size_t int_log(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
	size_t const ARRAY_PARAM_LIM(powers_of_N, 1)
) {
	size_t const log2_of_x = int_log2(x);
	size_t const power_idx = (*logN_table)[log2_of_x];
	size_t const next_pow  = powers_of_N[power_idx + 1];
	return power_idx + (0 < next_pow && next_pow <= x);
}

HELPER_FUNC NO_NULL size_t make_int_log_tables(size_t const base, size_t (*const logN_table)[MAX_LOG_TABLE_SIZE], size_t **const restrict powers_of_N) {
	/// int_log2 is fast enough, no point generating table.
	if( base < 3 ) {
		return 0;
	}
	/// floor( log ~0 / log base ) === highest power of the base `size_t` or other int types can reach.
	size_t const num_exp = ( size_t )(log(SIZE_MAX) / log(base)) + 1;
	size_t *powers = calloc(num_exp + 1, sizeof *powers);
	if( powers==nullptr ) {
		return 0;
	}
	
	/// Generate our powers of the base.
	powers[0] = 1; /// log_N(1) = 0;
	for( size_t i=1; i < num_exp; i++ ) {
		size_t const prev_pow = powers[i-1];
		powers[i] = prev_pow * base;
	}
	*powers_of_N = powers;
	
	/// generate log of N table.
	for( size_t i=0; i < MAX_LOG_TABLE_SIZE; i++ ) {
		(*logN_table)[i] = floor(log(harbol_bit_index(i)) / log(base));
	}
	return num_exp;
}


HELPER_FUNC size_t base_N_digits(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
	size_t const ARRAY_PARAM_LIM(powers_of_N, 1)
) {
	return int_log(x, logN_table, powers_of_N) + 1;
}

HELPER_FUNC size_t base_N_num_chars_uint(
	size_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
	size_t const ARRAY_PARAM_LIM(powers_of_N, 1)
) {
	return base_N_digits(x, logN_table, powers_of_N) + 1;
}

HELPER_FUNC size_t base_N_num_chars_int(
	ssize_t const x,
	size_t const (*const logN_table)[MAX_LOG_TABLE_SIZE],
	size_t const ARRAY_PARAM_LIM(powers_of_N, 1)
) {
	return base_N_digits(int_abs(x), logN_table, powers_of_N) + 2; /// 2 for the negative sign symbol.
}

#endif /** HARBOL_INT_LOGARITHM_INCLUDED */