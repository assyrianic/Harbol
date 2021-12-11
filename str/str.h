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


HARBOL_EXPORT NEVER_NULL(2) struct HarbolString harbol_string_make(const char cstr[], bool *res);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_init(struct HarbolString *str, const char cstr[]);
HARBOL_EXPORT struct HarbolString *harbol_string_new(const char *cstr);

HARBOL_EXPORT NO_NULL void harbol_string_clear(struct HarbolString *str);
HARBOL_EXPORT NO_NULL void harbol_string_free(struct HarbolString **strref);

HARBOL_EXPORT NO_NULL const char *harbol_string_cstr(const struct HarbolString *str);
HARBOL_EXPORT NO_NULL char *harbol_string_mut_cstr(const struct HarbolString *str);
HARBOL_EXPORT NO_NULL size_t harbol_string_len(const struct HarbolString *str);

HARBOL_EXPORT NO_NULL bool harbol_string_add_char(struct HarbolString *str, char chr);
HARBOL_EXPORT NO_NULL bool harbol_string_add_str(struct HarbolString *strA, const struct HarbolString *strB);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_add_cstr(struct HarbolString *str, const char cstr[]);

#ifdef C11
#	define harbol_string_add(str, val)  _Generic((val)+0, \
											char                       : harbol_string_add_char, \
											struct HarbolString*       : harbol_string_add_str, \
											const struct HarbolString* : harbol_string_add_str, \
											char*                      : harbol_string_add_cstr, \
											const char*                : harbol_string_add_cstr) \
										((str), (val))
#endif


HARBOL_EXPORT NO_NULL bool harbol_string_copy_str(struct HarbolString *strA, const struct HarbolString *strB);
HARBOL_EXPORT NEVER_NULL(1) bool harbol_string_copy_cstr(struct HarbolString *str, const char cstr[]);

#ifdef C11
#	define harbol_string_copy(str, val) _Generic((val)+0, \
											struct HarbolString*       : harbol_string_copy_str, \
											const struct HarbolString* : harbol_string_copy_str, \
											char*                      : harbol_string_copy_cstr, \
											const char*                : harbol_string_copy_cstr) \
										((str), (val))
#endif

HARBOL_EXPORT NEVER_NULL(1, 3) int harbol_string_format(struct HarbolString *str, bool clear, const char fmt[], ...);
HARBOL_EXPORT NEVER_NULL(1, 2) int harbol_string_scan(const struct HarbolString *str, const char fmt[], ...);

HARBOL_EXPORT NEVER_NULL(1) int harbol_string_cmpcstr(const struct HarbolString *str, const char cstr[]);
HARBOL_EXPORT NO_NULL int harbol_string_cmpstr(const struct HarbolString *strA, const struct HarbolString *strB);

#ifdef C11
#	define harbol_string_cmp(str, val)  _Generic((val)+0, \
											struct HarbolString*       : harbol_string_cmpstr, \
											const struct HarbolString* : harbol_string_cmpstr, \
											char*                      : harbol_string_cmpcstr, \
											const char*                : harbol_string_cmpcstr) \
										((str), (val))
#endif

HARBOL_EXPORT NO_NULL bool harbol_string_empty(const struct HarbolString *str);
HARBOL_EXPORT NO_NULL bool harbol_string_is_palindrome(const struct HarbolString *str);

HARBOL_EXPORT NO_NULL bool harbol_string_read_from_file(struct HarbolString *str, FILE *file);
HARBOL_EXPORT NO_NULL bool harbol_string_read_file(struct HarbolString *str, const char filename[]);

HARBOL_EXPORT NO_NULL bool harbol_string_replace(struct HarbolString *str, char to_replace, char with);

HARBOL_EXPORT NO_NULL size_t harbol_string_count(const struct HarbolString *str, char occurrence);

HARBOL_EXPORT NO_NULL bool harbol_string_upper(struct HarbolString *str);
HARBOL_EXPORT NO_NULL bool harbol_string_lower(struct HarbolString *str);
HARBOL_EXPORT NO_NULL bool harbol_string_reverse(struct HarbolString *str);
HARBOL_EXPORT NO_NULL size_t harbol_string_rm_char(struct HarbolString *str, char c);


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_STRING_INCLUDED */
