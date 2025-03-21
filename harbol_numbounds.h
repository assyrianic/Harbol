#ifndef HARBOL_NUMBOUNDS_INCLUDED
#	define HARBOL_NUMBOUNDS_INCLUDED


HELPER_FUNC uintmax_t harbol_umax_max(uintmax_t const a, uintmax_t const b) { return a > b? a : b; }
HELPER_FUNC intmax_t  harbol_imax_max(intmax_t  const a, intmax_t  const b) { return a > b? a : b; }

HELPER_FUNC uintmax_t harbol_umax_min(uintmax_t const a, uintmax_t const b) { return a > b? b : a; }
HELPER_FUNC intmax_t  harbol_imax_min(intmax_t  const a, intmax_t  const b) { return a > b? b : a; }

HELPER_FUNC uintmax_t harbol_umax_clamp(uintmax_t const val, uintmax_t const min, uintmax_t const max) {
	return harbol_umax_min(harbol_umax_max(val, min), max);
}
HELPER_FUNC intmax_t  harbol_imax_clamp(intmax_t const val, intmax_t const min, intmax_t const max) {
	return harbol_imax_min(harbol_imax_max(val, min), max);
}


HELPER_FUNC bool is_uint_in_bounds(size_t const val, size_t const max, size_t const min) {
	return (val - min) <= (max - min);
}

HELPER_FUNC bool is_int_in_bounds(ssize_t const val, ssize_t const max, ssize_t const min) {
	return is_uint_in_bounds(( size_t )(val), ( size_t )(max), ( size_t )(min));
}

HELPER_FUNC size_t get_byte_diff(void const *const restrict a, void const *const restrict b) {
	uint8_t const *const restrict A = a;
	uint8_t const *const restrict B = b;
	return ( size_t )(A - B);
}

HELPER_FUNC bool bytes_within_bounds(void const *const restrict a, void const *const restrict b, size_t const len) {
	return get_byte_diff(a, b) < len;
}

HELPER_FUNC NO_NULL bool is_ptr_in_bounds(void const *const val, void const *const max, void const *const min) {
	return (( uintptr_t )(val) - ( uintptr_t )(min)) <= (( uintptr_t )(max) - ( uintptr_t )(min));
}

HELPER_FUNC NEVER_NULL(1,2) size_t csubstr_diff(char const ARRAY_PARAM_FULL(cstr, 1), char const ARRAY_PARAM_FULL(csubstr, 1), char const end[]) {
	char const *const pos = strstr(cstr, csubstr);
	return( pos==nullptr || (end != nullptr && pos >= end) )? SIZE_MAX : ( size_t )(pos - cstr);
}

HELPER_FUNC NEVER_NULL(2,3) bool csubstr_within_bounds(size_t const cstr_len, char const ARRAY_PARAM_FULL(cstr, cstr_len), char const ARRAY_PARAM_FULL(csubstr, 1), char const end[]) {
	return csubstr_diff(cstr, csubstr, end) < cstr_len;
}

#endif /** HARBOL_NUMBOUNDS_INCLUDED */