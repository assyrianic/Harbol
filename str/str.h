#ifndef HARBOL_STRING_INCLUDED
#	define HARBOL_STRING_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


struct HarbolString {
	char  *cstr;
	size_t len;
};


HARBOL_EXPORT NEVER_NULL(2) struct HarbolString harbol_string_make(char const cstr[], bool *res);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_init(struct HarbolString *str, char const cstr[]);
HARBOL_EXPORT struct HarbolString *harbol_string_new(char const *cstr);

HARBOL_EXPORT NO_NULL void harbol_string_clear(struct HarbolString *str);
HARBOL_EXPORT NO_NULL void harbol_string_free(struct HarbolString **strref);

HARBOL_EXPORT NO_NULL char const *harbol_string_cstr(struct HarbolString const *str);
HARBOL_EXPORT NO_NULL char *harbol_string_mut_cstr(struct HarbolString const *str);
HARBOL_EXPORT NO_NULL size_t harbol_string_len(struct HarbolString const *str);

HARBOL_EXPORT NO_NULL bool harbol_string_add_char(struct HarbolString *str, char chr);
HARBOL_EXPORT NO_NULL bool harbol_string_add_char_rep(struct HarbolString *str, char c, size_t amount);
HARBOL_EXPORT NO_NULL bool harbol_string_add_str(struct HarbolString *strA, struct HarbolString const *strB);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_add_cstr(struct HarbolString *str, char const cstr[]);

#ifdef C11
#	define harbol_string_add(str, val)  _Generic((val)+0, \
											char                       : harbol_string_add_char, \
											struct HarbolString*       : harbol_string_add_str, \
											struct HarbolString const* : harbol_string_add_str, \
											char*                      : harbol_string_add_cstr, \
											char const*                : harbol_string_add_cstr) \
										((str), (val))
#endif


HARBOL_EXPORT NO_NULL bool harbol_string_copy_str(struct HarbolString *strA, struct HarbolString const *strB);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_copy_cstr(struct HarbolString *str, char const cstr[]);

#ifdef C11
#	define harbol_string_copy(str, val) _Generic((val)+0, \
											struct HarbolString*       : harbol_string_copy_str, \
											struct HarbolString const* : harbol_string_copy_str, \
											char*                      : harbol_string_copy_cstr, \
											char const*                : harbol_string_copy_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT NEVER_NULL(1, 3) int harbol_string_format(struct HarbolString *str, bool clear, char const fmt[], ...);
HARBOL_EXPORT NEVER_NULL(1, 3) int harbol_string_format_va(struct HarbolString *str, bool clear, char const fmt[], va_list va_l);
HARBOL_EXPORT NEVER_NULL(1, 2) int harbol_string_scan(struct HarbolString const *str, char const fmt[], ...);
HARBOL_EXPORT NEVER_NULL(1, 2) int harbol_string_scan_va(struct HarbolString const *str, char const fmt[], va_list va_l);

HARBOL_EXPORT NEVER_NULL(1) int harbol_string_cmpcstr(struct HarbolString const *str, char const cstr[]);
HARBOL_EXPORT NO_NULL int harbol_string_cmpstr(struct HarbolString const *strA, struct HarbolString const *strB);

#ifdef C11
#	define harbol_string_cmp(str, val)  _Generic((val)+0, \
											struct HarbolString*       : harbol_string_cmpstr, \
											struct HarbolString const* : harbol_string_cmpstr, \
											char*                      : harbol_string_cmpcstr, \
											char const*                : harbol_string_cmpcstr) \
										((str), (val))
#endif

HARBOL_EXPORT NO_NULL bool harbol_string_empty(struct HarbolString const *str);
HARBOL_EXPORT NO_NULL bool harbol_string_is_palindrome(struct HarbolString const *str);

HARBOL_EXPORT NO_NULL bool harbol_string_read_from_file(struct HarbolString *str, FILE *file);
HARBOL_EXPORT NO_NULL bool harbol_string_read_file(struct HarbolString *str, char const filename[]);

HARBOL_EXPORT NO_NULL bool harbol_string_replace_char(struct HarbolString *str, char to_replace, char with);
HARBOL_EXPORT NO_NULL bool harbol_string_replace_cstr(struct HarbolString *str, char const to_replace[], char const with[], size_t amount);

HARBOL_EXPORT NO_NULL size_t harbol_string_count_char(struct HarbolString const *str, char occurrence);
HARBOL_EXPORT NO_NULL size_t harbol_string_count_cstr(struct HarbolString const *str, char const occurrence[]);
HARBOL_EXPORT NO_NULL bool harbol_string_cstr_offsets(struct HarbolString const *str, char const occurrence[], size_t offsets[], size_t offsets_len);

HARBOL_EXPORT NO_NULL bool harbol_string_upper(struct HarbolString *str);
HARBOL_EXPORT NO_NULL bool harbol_string_lower(struct HarbolString *str);
HARBOL_EXPORT NO_NULL bool harbol_string_reverse(struct HarbolString *str);
HARBOL_EXPORT NO_NULL size_t harbol_string_rm_char(struct HarbolString *str, char c);
HARBOL_EXPORT NO_NULL size_t harbol_string_rm_all_space(struct HarbolString *str);

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_STRING_INCLUDED */
