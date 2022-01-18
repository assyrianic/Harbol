#ifndef HARBOL_LEX_INCLUDED
#	define HARBOL_LEX_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"
#include "../str/str.h"
#include <ctype.h>

enum {
	DigitSep_C  = '\'',
	DigitSep_Go = '_'
};

HARBOL_EXPORT bool is_alphabetic(int32_t c);
HARBOL_EXPORT bool is_possible_id(int32_t c);
HARBOL_EXPORT bool is_decimal(int32_t c);
HARBOL_EXPORT bool is_octal(int32_t c);
HARBOL_EXPORT bool is_hex(int32_t c);
HARBOL_EXPORT bool is_binary(int32_t c);
HARBOL_EXPORT bool is_whitespace(int32_t c);

HARBOL_EXPORT bool is_valid_unicode(int32_t u);
HARBOL_EXPORT bool check_is_char(const char str[], size_t len, size_t idx, int32_t c);

HARBOL_EXPORT size_t get_utf8_len(char c);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_chars(const char str[], bool checker(int32_t c), size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_chars_until_newline(const char str[], bool checker(int32_t c));

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_string_literal(const char str[], char esc);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_single_line_comment(const char str[], size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_multi_line_comment(const char str[], const char end_token[], size_t end_len, size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET char *clear_single_line_comment(char str[]);

HARBOL_EXPORT NO_NULL NONNULL_RET char *clear_multi_line_comment(char str[], const char end_token[], size_t end_len);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *skip_multiquote_string(const char str[], const char quote[], size_t quote_len, char esc);

HARBOL_EXPORT NO_NULL bool lex_single_line_comment(const char str[], const char **end, struct HarbolString *buf, size_t *lines);
HARBOL_EXPORT NO_NULL bool lex_multi_line_comment(const char str[], const char **end, const char end_token[], size_t end_len, struct HarbolString *buf, size_t *lines);

HARBOL_EXPORT NO_NULL size_t write_utf8_cstr(char buf[], size_t buflen, int32_t rune);
HARBOL_EXPORT NO_NULL bool write_utf8_str(struct HarbolString *str, int32_t rune);
HARBOL_EXPORT NO_NULL size_t read_utf8(const char cstr[], size_t cstrlen, int32_t *rune);

HARBOL_EXPORT NO_NULL int32_t *utf8_to_rune(const struct HarbolString *str, size_t *rune_len);
HARBOL_EXPORT NO_NULL char *rune_to_utf8_cstr(const int32_t runes[], size_t rune_len, size_t *cstr_len);
HARBOL_EXPORT NO_NULL struct HarbolString rune_to_utf8_str(const int32_t runes[], size_t rune_len);

HARBOL_EXPORT NO_NULL int32_t lex_hex_escape_char(const char str[], const char **end);
HARBOL_EXPORT NO_NULL int32_t lex_octal_escape_char(const char str[], const char **end);
HARBOL_EXPORT NO_NULL int32_t lex_unicode_char(const char str[], const char **end, size_t encoding);


enum {
	HarbolLexNoErr,
	HarbolLexEoF,
	HarbolLexMissing0,
	HarbolLexMissingX,
	HarbolLexMissingO,
	HarbolLexMissingB,
	HarbolLexMissingHexB4dot,
	HarbolLexTooManyPs,
	HarbolLexBadPlusMinusPlace,
	HarbolLexNoNumAfterExp,
	HarbolLexTooManyUs,
	HarbolLexTooManyLs,
	HarbolLexUBetweenLs,
	HarbolLexHexFltExpNoDigits,
	HarbolLexHexFltBadSuffix,
	HarbolLexIntExtraSuffix,
	HarbolLexFltExtraSuffix,
	HarbolLexBadGylph,
	HarbolLexHexMissingDigits,
	HarbolLexHexNoDigitsB4Exp,
	HarbolLexDigitSepNearDot,
	HarbolLexDigitSepNearExp,
	HarbolLexExtraDigitSeps,
	HarbolLexHexFltNoExp,
	HarbolLexDigitSepMIADigits,
	HarbolLexExtraFltDot,
	HarbolLexExtraFltSuffix,
	HarbolLexExtraExp,
	HarbolLexMissingDotOrExp,
	HarbolLexExpAfterFltSuffix,
	HarbolLexFltSuffixAfterExpNoDigits,
	HarbolLexIntSuffixOnFlt,
	HarbolLexBadOctalChar,
	HarbolLexBadHexChar,
	HarbolLexBadUnicodeChar,
	HarbolLexSuddenEoFStr,
	HarbolLexDigitSepNotSepDigits,
};

HARBOL_EXPORT NO_NULL int lex_c_style_hex(const char str[], const char **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_hex(const char str[], const char **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_octal(const char str[], const char **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_octal(const char str[], const char **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL int lex_c_style_binary(const char str[], const char **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL int lex_go_style_binary(const char str[], const char **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL int lex_c_style_decimal(const char str[], const char **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_decimal(const char str[], const char **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_number(const char str[], const char **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_number(const char str[], const char **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_str(const char str[], const char **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL int lex_go_style_str(const char str[], const char **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL NONNULL_RET const char *lex_get_err(const int err_code);

HARBOL_EXPORT NO_NULL bool lex_identifier(const char str[], const char **end, struct HarbolString *buf, bool checker(int32_t c));
HARBOL_EXPORT NO_NULL bool lex_identifier_utf8(const char str[], const char **end, struct HarbolString *buf, bool checker(int32_t c));
HARBOL_EXPORT NO_NULL bool lex_c_style_identifier(const char str[], const char **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL bool lex_until(const char str[], const char **end, struct HarbolString *buf, int32_t control);

HARBOL_EXPORT NEVER_NULL(1) intmax_t lex_c_string_to_int(const struct HarbolString *buf, char **end);
HARBOL_EXPORT NEVER_NULL(1) intmax_t lex_go_string_to_int(const struct HarbolString *buf, char **end);
HARBOL_EXPORT NEVER_NULL(1) uintmax_t lex_c_string_to_uint(const struct HarbolString *buf, char **end);
HARBOL_EXPORT NEVER_NULL(1) uintmax_t lex_go_string_to_uint(const struct HarbolString *buf, char **end);
HARBOL_EXPORT NO_NULL floatmax_t lex_string_to_float(const struct HarbolString *buf);
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_LEX_INCLUDED */
