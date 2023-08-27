#include <ctype.h>
#include "str.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

static NO_NULL bool _harbol_resize_string(struct HarbolString *const restrict str, size_t const new_size) {
	char *restrict new_cstr = harbol_recalloc(str->cstr, new_size + 1, sizeof *new_cstr, str->len);
	if( new_cstr==NULL ) {
		return false;
	}
	str->cstr = new_cstr;
	str->len  = new_size;
	return true;
}

HARBOL_EXPORT struct HarbolString *harbol_string_new(char const cstr[]) {
	struct HarbolString *restrict str = calloc(1, sizeof *str);
	if( str==NULL ) {
		return NULL;
	}
	harbol_string_init(str, cstr);
	return str;
}

HARBOL_EXPORT struct HarbolString harbol_string_make(char const cstr[], bool *const restrict res) {
	struct HarbolString s = {0};
	*res = harbol_string_init(&s, cstr);
	return s;
}

HARBOL_EXPORT bool harbol_string_init(struct HarbolString *const restrict str, char const cstr[]) {
	harbol_string_copy_cstr(str, cstr);
	return str->cstr != NULL;
}

HARBOL_EXPORT void harbol_string_clear(struct HarbolString *const str) {
	free(str->cstr); str->cstr = NULL;
	str->len = 0;
}

HARBOL_EXPORT void harbol_string_free(struct HarbolString **const strref) {
	if( *strref==NULL ) {
		return;
	}
	harbol_string_clear(*strref);
	free(*strref); *strref = NULL;
}

HARBOL_EXPORT char const *harbol_string_cstr(struct HarbolString const *const str) {
	return str->cstr;
}
HARBOL_EXPORT char *harbol_string_mut_cstr(struct HarbolString const *const str) {
	return str->cstr;
}
HARBOL_EXPORT size_t harbol_string_len(struct HarbolString const *const str) {
	return str->len;
}

HARBOL_EXPORT bool harbol_string_add_char(struct HarbolString *const str, char const c) {
	if( !_harbol_resize_string(str, str->len + 1) ) {
		return false;
	}
	str->cstr[str->len-1] = c;
	return true;
}

HARBOL_EXPORT bool harbol_string_add_char_rep(struct HarbolString *const str, char const c, size_t const amount) {
	if( !_harbol_resize_string(str, str->len + amount) ) {
		return false;
	}
	for( size_t i=0; i < amount; i++ ) {
		str->cstr[str->len-i-1] = c;
	}
	return true;
}

HARBOL_EXPORT bool harbol_string_add_str(struct HarbolString *const strA, struct HarbolString const *const strB) {
	if( strB->cstr==NULL || !_harbol_resize_string(strA, strA->len + strB->len) ) {
		return false;
	}
	strncat(strA->cstr, strB->cstr, strB->len);
	return true;
}

HARBOL_EXPORT bool harbol_string_add_cstr(struct HarbolString *const restrict str, char const *restrict cstr) {
	if( cstr==NULL ) {
		return false;
	}
	
	size_t const cstr_len = strlen(cstr);
	if( !_harbol_resize_string(str, str->len + cstr_len) ) {
		return false;
	}
	strcat(str->cstr, cstr);
	return true;
}

#ifdef C11
#	define harbol_string_add(str, val)  _Generic((val)+0, \
											char                       : harbol_string_add_char, \
											struct HarbolString*       : harbol_string_add_str, \
											struct HarbolString const* : harbol_string_add_str, \
											char*                      : harbol_string_add_cstr, \
											char const*                : harbol_string_add_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT bool harbol_string_copy_str(struct HarbolString *const strA, struct HarbolString const *const strB) {
	if( strA==strB ) {
		return true;
	} else if( strB->cstr==NULL || !_harbol_resize_string(strA, strB->len) ) {
		return false;
	}
	strcpy(strA->cstr, strB->cstr);
	return true;
}

HARBOL_EXPORT bool harbol_string_copy_cstr(struct HarbolString *const restrict str, char const cstr[]) {
	if( cstr==NULL ) {
		return false;
	}
	
	size_t const cstr_len = strlen(cstr);
	if( cstr_len==0 || !_harbol_resize_string(str, cstr_len) ) {
		return false;
	}
	strcpy(str->cstr, cstr);
	return true;
}

#ifdef C11
#	define harbol_string_copy(str, val) _Generic((val)+0, \
											struct HarbolString*       : harbol_string_copy_str, \
											struct HarbolString const* : harbol_string_copy_str, \
											char*                      : harbol_string_copy_cstr, \
											char const*                : harbol_string_copy_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT int harbol_string_format(struct HarbolString *const restrict str, bool const clear, char const fmt[static 1], ...) {
	va_list ap; va_start(ap, fmt);
	return harbol_string_format_va(str, clear, fmt, ap);
}

HARBOL_EXPORT int harbol_string_format_va(struct HarbolString *const restrict str, bool const clear, char const fmt[static 1], va_list va_l) {
	va_list st; va_copy(st, va_l);
	char c = 0;
	int const size = vsnprintf(&c, 1, fmt, va_l);
	va_end(va_l);
	
	size_t const old_size = clear? 0 : str->len;
	if( !_harbol_resize_string(str, size + old_size) ) {
		va_end(st);
		return -1;
	}
	/// vsnprintf always checks n-1 so gotta increase len a bit to accomodate.
	int const result = vsnprintf(&str->cstr[old_size], (str->len - old_size) + 2, fmt, st);
	va_end(st);
	return result;
}


HARBOL_EXPORT int harbol_string_scan(struct HarbolString const *const restrict str, char const fmt[const restrict static 1], ...) {
	va_list args; va_start(args, fmt);
	return harbol_string_scan_va(str, fmt, args);
}


HARBOL_EXPORT int harbol_string_scan_va(struct HarbolString const *const restrict str, char const fmt[const restrict static 1], va_list args) {
	int const result = vsscanf(str->cstr, fmt, args);
	va_end(args);
	return result;
}


HARBOL_EXPORT int harbol_string_cmpcstr(struct HarbolString const *const str, char const cstr[]) {
	if( cstr==NULL || str->cstr==NULL ) {
		return -1;
	}
	size_t const cstr_len = strlen(cstr);
	return strncmp(cstr, str->cstr, (str->len > cstr_len)? str->len : cstr_len);
}

HARBOL_EXPORT int harbol_string_cmpstr(struct HarbolString const *const strA, struct HarbolString const *const strB) {
	return( strA->cstr==NULL || strB->cstr==NULL )? -1 : strncmp(strA->cstr, strB->cstr, strA->len > strB->len? strA->len : strB->len);
}

HARBOL_EXPORT bool harbol_string_empty(struct HarbolString const *const str) {
	return( str->cstr==NULL || str->len==0 || str->cstr[0]==0 );
}

HARBOL_EXPORT bool harbol_string_is_palindrome(struct HarbolString const *const str) {
	if( harbol_string_empty(str) ) {
		return false;
	}
	size_t const half_len = str->len / 2;
	for( size_t i=0; i < half_len; i++ ) {
		if( str->cstr[i] != str->cstr[str->len - i - 1] ) {
			return false;
		}
	}
	return true;
}

HARBOL_EXPORT bool harbol_string_read_from_file(struct HarbolString *const str, FILE *const file) {
	ssize_t const filesize = get_file_size(file);
	if( filesize<=0 || !_harbol_resize_string(str, filesize) ) {
		return false;
	}
	str->len = fread(str->cstr, sizeof *str->cstr, filesize, file);
	return true;
}

HARBOL_EXPORT bool harbol_string_read_file(struct HarbolString *const restrict str, char const filename[static 1]) {
	FILE *restrict file = fopen(filename, "r");
	if( file==NULL ) {
		return false;
	}
	bool const read_result = harbol_string_read_from_file(str, file);
	fclose(file);
	return read_result;
}

HARBOL_EXPORT bool harbol_string_replace_char(struct HarbolString *const str, char const to_replace, char const with) {
	if( str->cstr==NULL || to_replace==0 || with==0 ) {
		return false;
	}
	bool got_something = false;
	for( size_t i=0; i < (str->len + 1); i++ ) {
		if( str->cstr[i]==to_replace ) {
			str->cstr[i] = with;
			got_something |= true;
		}
	}
	return got_something;
}


static NO_NULL size_t _subcstr_diff(char const src[const restrict static 1], char const cstr[const restrict static 1]) {
	char const *const pos = strstr(src, cstr);
	return( pos==NULL )? SIZE_MAX : ( size_t )(pos - src);
}

HARBOL_EXPORT bool harbol_string_replace_cstr(struct HarbolString *const restrict str, char const to_replace[const restrict static 1], char const with[const restrict static 1], size_t amount) {
	if( str->cstr==NULL ) {
		return false;
	}
	
	size_t const counts = harbol_string_count_cstr(str, to_replace);
	if( counts==0 ) {
		return false;
	} else if( amount > counts ) {
		amount = counts;
	}
	
	size_t const replace_len = strlen(to_replace);
	size_t const with_len    = strlen(with);
	struct HarbolString rep_str = {0};
	
	size_t offset = _subcstr_diff(&str->cstr[0], to_replace);
	size_t const len_calc = str->len + (amount * (with_len - replace_len));
	_harbol_resize_string(&rep_str, len_calc);
	size_t rep_len = 0;
	/// first copy contents up to the first offset.
	strncpy(&rep_str.cstr[0], &str->cstr[0], offset);
	rep_len += offset;
	offset += replace_len;
	
	strcpy(&rep_str.cstr[rep_len], with);
	rep_len += with_len;
	for( size_t i=0; i < amount; i++ ) {
		size_t const saved_offset = offset;
		size_t const relative_offs = _subcstr_diff(&str->cstr[saved_offset], to_replace);
		if( relative_offs==SIZE_MAX ) {
			break;
		}
		
		offset += relative_offs;
		size_t const span = offset - saved_offset;
		strncpy(&rep_str.cstr[rep_len], &str->cstr[saved_offset], span);
		rep_len += span;
		
		strcpy(&rep_str.cstr[rep_len], with);
		rep_len += with_len;
		offset += replace_len;
	}
	strcpy(&rep_str.cstr[rep_len], &str->cstr[offset]);
	rep_len += offset;
	
	harbol_string_clear(str);
	str->cstr = rep_str.cstr;
	str->len  = rep_len;
	return true;
}

HARBOL_EXPORT size_t harbol_string_count_char(struct HarbolString const *const str, char const occurrence) {
	if( str->cstr==NULL ) {
		return 0;
	}
	
	size_t counts = 0;
	for( size_t i=0; i < (str->len + 1); i++ ) {
		if( str->cstr[i]==occurrence ) {
			counts++;
		}
	}
	return counts;
}

HARBOL_EXPORT size_t harbol_string_count_cstr(struct HarbolString const *const restrict str, char const occurrence[const restrict static 1]) {
	if( str->cstr==NULL ) {
		return false;
	}
	
	size_t occurrences = 0;
	size_t const occ_len = strlen(occurrence);
	char const *pos = strstr(str->cstr, occurrence);
	while( pos != NULL ) {
		occurrences++;
		pos = strstr(pos + occ_len, occurrence);
	}
	return occurrences;
}

HARBOL_EXPORT NO_NULL bool harbol_string_cstr_offsets(struct HarbolString const *const restrict str, char const occurrence[const restrict static 1], size_t offsets[const restrict static 1], size_t const offsets_len) {
	if( str->cstr==NULL ) {
		return false;
	}
	
	size_t offset = 0;
	for( size_t i=0; i < offsets_len; i++ ) {
		size_t const curr_offs = offset;
		offset += _subcstr_diff(&str->cstr[curr_offs], occurrence);
		offsets[i] = offset;
		offset++;
	}
	return true;
}

HARBOL_EXPORT bool harbol_string_upper(struct HarbolString *const str) {
	if( str->cstr==NULL ) {
		return false;
	}
	bool got_something = false;
	for( size_t i=0; i < (str->len + 1); i++ ) {
		if( islower(str->cstr[i]) ) {
			str->cstr[i] = toupper(str->cstr[i]);
			got_something |= true;
		}
	}
	return got_something;
}

HARBOL_EXPORT bool harbol_string_lower(struct HarbolString *const str) {
	if( str->cstr==NULL ) {
		return false;
	}
	bool got_something = false;
	for( size_t i=0; i < (str->len + 1); i++ ) {
		if( isupper(str->cstr[i]) ) {
			str->cstr[i] = tolower(str->cstr[i]);
			got_something |= true;
		}
	}
	return got_something;
}

HARBOL_EXPORT bool harbol_string_reverse(struct HarbolString *const str) {
	if( str->cstr==NULL ) {
		return false;
	}
	bool got_something = false;
	size_t const half_len = str->len / 2;
	for( size_t i=0, n=str->len-1; i < half_len; i++, n-- ) {
		int_fast8_t const t = str->cstr[i];
		str->cstr[i] = str->cstr[n];
		str->cstr[n] = t;
		got_something |= true;
	}
	return got_something;
}


HARBOL_EXPORT size_t harbol_string_rm_char(struct HarbolString *const str, char const c) {
	size_t j = 0, counts = 0;
	for( size_t i=0; str->cstr[i] != 0; i++ ) {
		if( str->cstr[i] != c ) {
			str->cstr[j++] = str->cstr[i];
		} else {
			counts++;
		}
	}
	str->cstr[j] = 0;
	return counts;
}


HARBOL_EXPORT size_t harbol_string_rm_all_space(struct HarbolString *const str) {
	size_t j = 0, counts = 0;
	for( size_t i=0; str->cstr[i] != 0; i++ ) {
		if( !isspace(str->cstr[i]) ) {
			str->cstr[j++] = str->cstr[i];
		} else {
			counts++;
			str->len--;
		}
	}
	str->cstr[j] = 0;
	return counts;
}
