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
	DigitSep_Go = '_',
};

HARBOL_EXPORT bool is_alphabetic(int32_t c);
HARBOL_EXPORT bool is_possible_id(int32_t c);
HARBOL_EXPORT bool is_decimal(int32_t c);
HARBOL_EXPORT bool is_octal(int32_t c);
HARBOL_EXPORT bool is_hex(int32_t c);
HARBOL_EXPORT bool is_binary(int32_t c);
HARBOL_EXPORT bool is_whitespace(int32_t c);

HARBOL_EXPORT bool is_valid_unicode(int32_t u);
HARBOL_EXPORT NO_NULL bool check_is_char(char const str[], size_t len, size_t idx, int32_t c);
HARBOL_EXPORT NO_NULL bool check_is_rune(int32_t const str[], size_t len, size_t idx, int32_t c);

HARBOL_EXPORT size_t get_utf8_len(char c);
HARBOL_EXPORT NO_NULL size_t get_str_rune_len(char const cstr[]);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_chars(char const str[], bool checker(int32_t c), size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_chars_until_newline(char const str[], bool checker(int32_t c));

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_string_literal(char const str[], char esc);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_single_line_comment(char const str[], size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_multi_line_comment(char const str[], char const end_token[], size_t end_len, size_t *lines);

HARBOL_EXPORT NO_NULL NONNULL_RET char *clear_single_line_comment(char str[]);

HARBOL_EXPORT NO_NULL NONNULL_RET char *clear_multi_line_comment(char str[], char const end_token[], size_t end_len);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *skip_multiquote_string(char const str[], char const quote[], size_t quote_len, char esc);

HARBOL_EXPORT NO_NULL bool lex_single_line_comment(char const str[], char const **end, struct HarbolString *buf, size_t *lines);
HARBOL_EXPORT NO_NULL bool lex_multi_line_comment(char const str[], char const **end, char const end_token[], size_t end_len, struct HarbolString *buf, size_t *lines);

/// TODO:
HARBOL_EXPORT NO_NULL bool lex_multiquote_string(char const str[], char const **end, char const quote[], size_t quote_len, int32_t esc, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL size_t write_utf8_cstr(char buf[], size_t buflen, int32_t rune);
HARBOL_EXPORT NO_NULL bool write_utf8_str(struct HarbolString *str, int32_t rune);
HARBOL_EXPORT NO_NULL size_t read_utf8(char const cstr[], size_t cstrlen, int32_t *rune);
HARBOL_EXPORT NO_NULL int32_t read_utf8_rune(char const cstr[], size_t cstrlen);

HARBOL_EXPORT NO_NULL int32_t *utf8_to_rune(struct HarbolString const *str, size_t *rune_len);
HARBOL_EXPORT NO_NULL char *rune_to_utf8_cstr(int32_t const runes[], size_t rune_len, size_t *cstr_len);
HARBOL_EXPORT NO_NULL struct HarbolString rune_to_utf8_str(int32_t const runes[], size_t rune_len);

HARBOL_EXPORT NO_NULL int32_t lex_hex_escape_char(char const str[], char const **end);
HARBOL_EXPORT NO_NULL int32_t lex_octal_escape_char(char const str[], char const **end);
HARBOL_EXPORT NO_NULL int32_t lex_unicode_char(char const str[], char const **end, size_t encoding);


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

HARBOL_EXPORT NO_NULL int lex_c_style_hex(char const str[], char const **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_hex(char const str[], char const **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_octal(char const str[], char const **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_octal(char const str[], char const **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL int lex_c_style_binary(char const str[], char const **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL int lex_go_style_binary(char const str[], char const **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL int lex_c_style_decimal(char const str[], char const **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_decimal(char const str[], char const **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_number(char const str[], char const **end, struct HarbolString *buf, bool *is_float);
HARBOL_EXPORT NO_NULL int lex_go_style_number(char const str[], char const **end, struct HarbolString *buf, bool *is_float);

HARBOL_EXPORT NO_NULL int lex_c_style_str(char const str[], char const **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL int lex_go_style_str(char const str[], char const **end, struct HarbolString *buf);

HARBOL_EXPORT NO_NULL NONNULL_RET char const *lex_get_err(int const err_code);

HARBOL_EXPORT NO_NULL bool lex_identifier(char const str[], char const **end, struct HarbolString *buf, bool checker(int32_t c));
HARBOL_EXPORT NO_NULL bool lex_identifier_utf8(char const str[], char const **end, struct HarbolString *buf, bool checker(int32_t c));
HARBOL_EXPORT NO_NULL bool lex_c_style_identifier(char const str[], char const **end, struct HarbolString *buf);
HARBOL_EXPORT NO_NULL bool lex_until(char const str[], char const **end, struct HarbolString *buf, int32_t control);

HARBOL_EXPORT NEVER_NULL(1) intmax_t lex_c_string_to_int(struct HarbolString const *str, char **end);
HARBOL_EXPORT NEVER_NULL(1) intmax_t lex_go_string_to_int(struct HarbolString const *str, char **end);
HARBOL_EXPORT NEVER_NULL(1) uintmax_t lex_c_string_to_uint(struct HarbolString const *str, char **end);
HARBOL_EXPORT NEVER_NULL(1) uintmax_t lex_go_string_to_uint(struct HarbolString const *str, char **end);
HARBOL_EXPORT NO_NULL floatmax_t lex_string_to_float(struct HarbolString const *str);


/// TODO: finish this up.
/// Examples
/// 1, 1.0, .1, .3f, 0x23, 0o337, 0b010110, 1'2'3,
/// 1_2_3, 1e-3, 1ull, 1llu, 1u, 1ul
/// 0x0.1p3, 1i32, 1_i32, 9.81_m/s^2
enum LexingFlag {
	LexFlagRepeatable      = 1 << 0,
	LexFlagNumSeparator    = 1 << 1,
	LexFlagSymSeparator    = 1 << 2,
	LexFlagReqOneNumBefore = 1 << 3,
	LexFlagReqOneNumAfter  = 1 << 4,
	LexFlagMustBeLast      = 1 << 5,
	LexFlagNoNumBefore     = 1 << 6,
	LexFlagNoNumAfter      = 1 << 7,
	LexFlagReqOtherFlags   = 1 << 8,
	LexFlagSeparatorBefore = 1 << 9,
	LexFlagSeparatorAfter  = 1 << 10,
	LexFlagReqSymBefore    = 1 << 11,
	LexFlagReqSymAfter     = 1 << 12,
	LexFlagOptionNumAfter  = 1 << 13,
	LexFlagOptionSymAfter  = 1 << 14,
};

/// for LexFlagRepeatable
struct RepeatingSym {
	char const *sym;
	size_t      len;
	uint32_t    limit, matched;
};


struct LexingRules {
	//struct HarbolMap prefixes, suffixes; /// map[string]LexingFlag
	struct HarbolString valid_digits;
};

HARBOL_EXPORT NO_NULL bool lex_custom_number(char const str[], char const **end, struct LexingRules const *rules, struct HarbolString *buf);
/********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_LEX_INCLUDED */