#include <ctype.h>
#include "str.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif

static NO_NULL bool _harbol_resize_string(struct HarbolString *const restrict str, const size_t new_size)
{
	char *restrict new_cstr = harbol_recalloc(str->cstr, new_size + 1, sizeof *new_cstr, str->len);
	if( new_cstr==NULL ) {
		return false;
	}
	str->cstr = new_cstr;
	str->len = new_size;
	return true;
}

HARBOL_EXPORT struct HarbolString *harbol_string_new(const char cstr[])
{
	struct HarbolString *restrict str = calloc(1, sizeof *str);
	if( str==NULL ) {
		free(str); str = NULL;
	} else {
		harbol_string_init(str, cstr);
	}
	return str;
}

HARBOL_EXPORT struct HarbolString harbol_string_make(const char cstr[], bool *const restrict res) {
	struct HarbolString s = {0};
	*res = harbol_string_init(&s, cstr);
	return s;
}

HARBOL_EXPORT bool harbol_string_init(struct HarbolString *const restrict str, const char cstr[]) {
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

HARBOL_EXPORT const char *harbol_string_cstr(const struct HarbolString *const str) {
	return str->cstr;
}
HARBOL_EXPORT char *harbol_string_mut_cstr(const struct HarbolString *const str) {
	return str->cstr;
}
HARBOL_EXPORT size_t harbol_string_len(const struct HarbolString *const str) {
	return str->len;
}

HARBOL_EXPORT bool harbol_string_add_char(struct HarbolString *const str, const char c) {
	if( !_harbol_resize_string(str, str->len + 1) ) {
		return false;
	}
	str->cstr[str->len-1] = c;
	return true;
}

HARBOL_EXPORT bool harbol_string_add_str(struct HarbolString *const strA, const struct HarbolString *const strB) {
	if( strB->cstr==NULL || !_harbol_resize_string(strA, strA->len + strB->len) ) {
		return false;
	}
	strncat(strA->cstr, strB->cstr, strB->len);
	return true;
}

HARBOL_EXPORT bool harbol_string_add_cstr(struct HarbolString *const restrict str, const char *restrict cstr) {
	if( cstr==NULL ) {
		return false;
	}
	
	const size_t cstr_len = strlen(cstr);
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
											const struct HarbolString* : harbol_string_add_str, \
											char*                      : harbol_string_add_cstr, \
											const char*                : harbol_string_add_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT bool harbol_string_copy_str(struct HarbolString *const strA, const struct HarbolString *const strB) {
	if( strA==strB ) {
		return true;
	} else if( strB->cstr==NULL || !_harbol_resize_string(strA, strB->len) ) {
		return false;
	}
	strcpy(strA->cstr, strB->cstr);
	return true;
}

HARBOL_EXPORT bool harbol_string_copy_cstr(struct HarbolString *const restrict str, const char cstr[]) {
	if( cstr==NULL ) {
		return false;
	}
	
	const size_t cstr_len = strlen(cstr);
	if( !_harbol_resize_string(str, cstr_len) ) {
		return false;
	}
	strcpy(str->cstr, cstr);
	return true;
}

#ifdef C11
#	define harbol_string_copy(str, val) _Generic((val)+0, \
											struct HarbolString*       : harbol_string_copy_str, \
											const struct HarbolString* : harbol_string_copy_str, \
											char*                      : harbol_string_copy_cstr, \
											const char*                : harbol_string_copy_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT int harbol_string_format(struct HarbolString *const restrict str, const bool clear, const char fmt[static 1], ...) {
	va_list ap, st;
	va_start(ap, fmt);
	va_copy(st, ap);
	
	/// '*snprintf' family returns the size of how large the writing would be if the buffer was large enough.
	char c = 0;
	const int size = vsnprintf(&c, 1, fmt, ap);
	va_end(ap);
	
	const size_t old_size = clear ? 0 : str->len;
	if( !_harbol_resize_string(str, size + old_size) ) {
		va_end(st);
		return -1;
	}
	/// vsnprintf always checks n-1 so gotta increase len a bit to accomodate.
	const int result = vsnprintf(&str->cstr[old_size], (str->len - old_size) + 2, fmt, st);
	va_end(st);
	return result;
}

HARBOL_EXPORT int harbol_string_scan(const struct HarbolString *const str, const char fmt[static 1], ...) {
	va_list args;
	va_start(args, fmt);
	const int result = vsscanf(str->cstr, fmt, args);
	va_end(args);
	return result;
}

HARBOL_EXPORT int harbol_string_cmpcstr(const struct HarbolString *const str, const char cstr[]) {
	if( cstr==NULL || str->cstr==NULL ) {
		return -1;
	}
	
	const size_t cstr_len = strlen(cstr);
	return strncmp(cstr, str->cstr, (str->len > cstr_len) ? str->len : cstr_len);
}

HARBOL_EXPORT int harbol_string_cmpstr(const struct HarbolString *const strA, const struct HarbolString *const strB) {
	return( strA->cstr==NULL || strB->cstr==NULL ) ? -1 : strncmp(strA->cstr, strB->cstr, strA->len > strB->len ? strA->len : strB->len);
}

#ifdef C11
#	define harbol_string_cmp(str, val)  _Generic((val)+0, \
											struct HarbolString*       : harbol_string_cmpstr, \
											const struct HarbolString* : harbol_string_cmpstr, \
											char*                      : harbol_string_cmpcstr, \
											const char*                : harbol_string_cmpcstr) \
										((str), (val))
#endif

HARBOL_EXPORT bool harbol_string_empty(const struct HarbolString *const str) {
	return( str->cstr==NULL || str->len==0 || str->cstr[0]==0 );
}

HARBOL_EXPORT bool harbol_string_is_palindrome(const struct HarbolString *const str) {
	if( harbol_string_empty(str) ) {
		return false;
	}
	const size_t half_len = str->len / 2;
	for( size_t i=0; i<half_len; i++ ) {
		if( str->cstr[i] != str->cstr[str->len - i - 1] ) {
			return false;
		}
	}
	return true;
}

HARBOL_EXPORT bool harbol_string_read_from_file(struct HarbolString *const str, FILE *const file) {
	const ssize_t filesize = get_file_size(file);
	if( filesize<=0 || !_harbol_resize_string(str, filesize) ) {
		return false;
	}
	str->len = fread(str->cstr, sizeof *str->cstr, filesize, file);
	return true;
}

HARBOL_EXPORT bool harbol_string_read_file(struct HarbolString *const restrict str, const char filename[static 1]) {
	FILE *restrict file = fopen(filename, "r");
	if( file==NULL )
		return false;
	
	const bool read_result = harbol_string_read_from_file(str, file);
	fclose(file);
	return read_result;
}

HARBOL_EXPORT bool harbol_string_replace(struct HarbolString *const str, const char to_replace, const char with) {
	if( str->cstr==NULL || to_replace==0 || with==0 ) {
		return false;
	}
	bool got_something = false;
	for( size_t i=0; i < str->len+1; i++ ) {
		if( str->cstr[i]==to_replace ) {
			str->cstr[i] = with;
			got_something |= true;
		}
	}
	return got_something;
}

HARBOL_EXPORT size_t harbol_string_count(const struct HarbolString *const str, const char occurrence) {
	if( str->cstr==NULL ) {
		return 0;
	}
	
	size_t counts = 0;
	for( size_t i=0; i < str->len+1; i++ ) {
		if( str->cstr[i]==occurrence ) {
			++counts;
		}
	}
	return counts;
}

HARBOL_EXPORT bool harbol_string_upper(struct HarbolString *const str) {
	if( str->cstr==NULL ) {
		return false;
	}
	bool got_something = false;
	for( size_t i=0; i<(str->len+1); i++ ) {
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
	for( size_t i=0; i<(str->len+1); i++ ) {
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
	const size_t half_len = str->len / 2;
	for( size_t i=0, n=str->len-1; i<half_len; i++, n-- ) {
		const int_fast8_t t = str->cstr[i];
		str->cstr[i] = str->cstr[n];
		str->cstr[n] = t;
		got_something |= true;
	}
	return got_something;
}


HARBOL_EXPORT size_t harbol_string_rm_char(struct HarbolString *const str, const char c) {
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
