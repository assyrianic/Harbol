#include "lex.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT bool is_alphabetic(int32_t const c) {
	return( (c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || c < -1 );
}
HARBOL_EXPORT bool is_possible_id(int32_t const c) {
	return( is_alphabetic(c) || (c>='0' && c<='9') );
}
HARBOL_EXPORT bool is_decimal(int32_t const c) {
	return( c>='0' && c<='9' );
}
HARBOL_EXPORT bool is_octal(int32_t const c) {
	return( c>='0' && c<='7' );
}
HARBOL_EXPORT bool is_hex(int32_t const c) {
	return( (c>='a' && c<='f') || (c>='A' && c<='F') || is_decimal(c) );
}
HARBOL_EXPORT bool is_binary(int32_t const c) {
	return( c=='0' || c=='1' );
}
HARBOL_EXPORT bool is_whitespace(int32_t const c) {
	return( c==' ' || c=='\t' || c=='\r' || c=='\v' || c=='\f' || c=='\n' );
}

HARBOL_EXPORT bool is_valid_unicode(int32_t const u) {
	uint32_t const c = ( uint32_t )(u);
	/// C11 6.4.3p2: U+D800 to U+DFFF are reserved for surrogate pairs.
	/// A codepoint within the range cannot be a valid character.
	if( 0xD800u <= c && c <= 0xDFFFu ) {
		return false;
	}
	/// It's not allowed to encode ASCII characters using \U or \u.
	/// Some characters not in the basic character set (C11 5.2.1p3)
	/// are allowed as exceptions.
	return 0xA0u <= c || c=='$' || c=='@' || c=='`';
}

HARBOL_EXPORT bool check_is_char(char const str[static 1], size_t const len, size_t const idx, int32_t const c) {
	return( idx >= len )? false : str[idx] != 0 && str[idx]==c;
}

HARBOL_EXPORT bool check_is_rune(int32_t const str[static 1], size_t const len, size_t const idx, int32_t const c) {
	return( idx >= len )? false : str[idx] != 0 && str[idx]==c;
}

HARBOL_EXPORT size_t get_utf8_len(char const c) {
	for( size_t i=7; i < 8; i-- ) {
		if( !(c & (1 << i)) ) {
			return (7 - i)==0? 1 : 7 - i;
		}
	}
	return 8;
}

HARBOL_EXPORT size_t get_str_rune_len(char const cstr[const static 1]) {
	size_t runes = 0;
	for( size_t i=0; cstr[i] != 0; ) {
		size_t const read = get_utf8_len(cstr[i]);
		i += read;
		runes++;
	}
	return runes;
}

HARBOL_EXPORT size_t find_rune_in_runes(int32_t const runes[static 1], int32_t const c) {
	for( size_t i=0; runes[i] != 0; i++ ) {
		if( c==runes[i] ) {
			return i;
		}
	}
	return SIZE_MAX;
}

HARBOL_EXPORT bool has_rune_in_runes(int32_t const runes[static 1], int32_t const c) {
	return find_rune_in_runes(runes, c) != SIZE_MAX;
}


HARBOL_EXPORT char const *skip_chars(char const str[static 1], bool checker(int32_t c), uint32_t *const restrict lines) {
	while( *str != 0 && checker(*str) ) {
		if( *str=='\n' ) {
			++*lines;
		}
		str++;
	}
	return str;
}

HARBOL_EXPORT char const *skip_chars_until_newline(char const str[static 1], bool checker(int32_t c)) {
	while( *str != 0 && *str != '\n' && checker(*str) ) {
		str++;
	}
	return str;
}

HARBOL_EXPORT char const *skip_string_literal(char const str[static 1], char const esc) {
	int_fast8_t const quote = *str++;
	while( *str != 0 && *str != quote ) {
		int_fast8_t const c = *str;
		str += ( c==esc )? 2 : 1;
	}
	return str;
}

HARBOL_EXPORT char const *skip_single_line_comment(char const str[static 1], uint32_t *const restrict lines) {
	char const *begin = str;
	while( *begin != 0 && *begin != '\n' ) {
		if( *begin=='\\' ) {
			while( *++begin != '\n' );
			if( *begin=='\n' ) {
				++*lines;
			}
		}
		begin++;
	}
	return begin;
}
HARBOL_EXPORT char const *skip_multi_line_comment(char const str[static 1], char const end_token[static 1], size_t const end_len, uint32_t *const restrict lines) {
	char const *begin = str + 1;
	while( *begin != 0 && strncmp(end_token, begin, end_len) != 0 ) {
		if( *begin=='\n' ) {
			++*lines;
		}
		begin++;
	}
	if( *begin != 0 ) {
		begin += end_len;
	}
	return begin;
}

HARBOL_EXPORT char *clear_single_line_comment(char str[static 1]) {
	char *begin = str;
	while( *begin != 0 && *begin != '\n' ) {
		if( *begin=='\\' ) {
			while( *++begin != '\n' );
			memset(str, ' ', begin-str);
			str = ++begin;
		} else {
			begin++;
		}
	}
	memset(str, ' ', begin-str);
	return begin;
}

HARBOL_EXPORT char *clear_multi_line_comment(char str[static 1], char const end_token[static 1], size_t const end_len) {
	char *begin = str + 1;
	while( *begin != 0 && strncmp(end_token, begin, end_len) != 0 ) {
		if( *begin=='\n' ) {
			memset(str, ' ', begin-str);
			str = ++begin;
		} else {
			begin++;
		}
	}
	if( *begin != 0 ) {
		begin += end_len;
	}
	memset(str, ' ', begin-str);
	return begin;
}

HARBOL_EXPORT char const *skip_multiquote_string(char const str[static 1], char const quote[static 1], size_t const quote_len, char const esc) {
	while( *str != 0 && strncmp(quote, str, quote_len) != 0 ) {
		int_fast8_t const c = *str;
		str += ( c==esc )? 2 : 1;
	}
	if( *str != 0 ) {
		str += quote_len;
	}
	return str;
}

HARBOL_EXPORT bool lex_single_line_comment(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, uint32_t *const restrict lines) {
	while( *str != 0 && *str != '\n' ) {
		if( *str=='\\' ) {
			harbol_string_add_char(buf, *str);
			while( *++str != '\n' ) {
				harbol_string_add_char(buf, *str);
			}
			if( *str=='\n' ) {
				++*lines;
			}
		}
		harbol_string_add_char(buf, *str++);
	}
	*end = str;
	return buf->len > 0;
}

HARBOL_EXPORT bool lex_multi_line_comment(char const str[static 1], char const **const end, char const end_token[static 1], size_t const end_len, struct HarbolString *const restrict buf, uint32_t *const restrict lines) {
	harbol_string_add_char(buf, *str++);
	while( *str != 0 && strncmp(end_token, str, end_len) != 0 ) {
		if( *str=='\n' ) {
			++*lines;
		}
		harbol_string_add_char(buf, *str++);
	}
	harbol_string_add_cstr(buf, end_token);
	if( *str != 0 ) {
		str += end_len;
	}
	*end = str;
	return buf->len > 0;
}

HARBOL_EXPORT bool lex_multiquote_string(char const str[static 1], char const **const end, char const quote[const restrict static 1], size_t quote_len, struct HarbolString *const restrict buf, size_t *const restrict line) {
	if( !strncmp(str, quote, quote_len) ) {
		str += quote_len;
	}
	while( str[0] != 0 && strncmp(str, quote, quote_len) ) {
		int_fast8_t const c = *str;
		if( c==0 ) {
			return false;
		} else if( c=='\n' ) {
			/// make sure you converted all carriage returns '\r' to '\n'.
			++*line;
			harbol_string_add_char(buf, c);
		}
		harbol_string_add_char(buf, *str);
		str++;
	}
	if( !strncmp(str, quote, quote_len) ) {
		str += quote_len;
	}
	*end = str;
	return true;
}

HARBOL_EXPORT size_t rune_byte_len(int32_t const rune) {
	if( rune < 0x80 ) {
		return 1;
	} else if( rune < 0x800 ) {
		return 2;
	} else if( rune < 0x10000 ) {
		return 3;
	} else if( rune < 0x110000 ) {
		return 4;
	}
	return 0;
}


HARBOL_EXPORT size_t write_utf8_cstr(char buf[static 1], size_t const buflen, int32_t const rune) {
	size_t const byte_len = rune_byte_len(rune);
	if( buflen < byte_len ) {
		return 0;
	}
	switch( byte_len ) {
		case 1:
			buf[0] = rune;
			break;
		case 2:
			buf[0] = (0xC0 | (rune >> 6));
			buf[1] = (0x80 | (rune & 0x3F));
			break;
		case 3:
			buf[0] = (0xE0 | (rune >> 12));
			buf[1] = (0x80 | ((rune >> 6) & 0x3F));
			buf[2] = (0x80 | (rune & 0x3F));
			break;
		case 4:
			buf[0] = (0xF0 | (rune >> 18));
			buf[1] = (0x80 | ((rune >> 12) & 0x3F));
			buf[2] = (0x80 | ((rune >> 6) & 0x3F));
			buf[3] = (0x80 | (rune & 0x3F));
			break;
		default:
			return 0;
	}
	return byte_len;
}

HARBOL_EXPORT bool write_utf8_str(struct HarbolString *const str, int32_t const rune) {
	size_t const byte_len = rune_byte_len(rune);
	switch( byte_len ) {
		case 1:
			harbol_string_add_char(str, rune);
			return true;
		case 2:
			harbol_string_add_char(str, 0xC0 | (rune >> 6));
			harbol_string_add_char(str, 0x80 | (rune & 0x3F));
			return true;
		case 3:
			harbol_string_add_char(str, 0xE0 | (rune >> 12));
			harbol_string_add_char(str, 0x80 | ((rune >> 6) & 0x3F));
			harbol_string_add_char(str, 0x80 | (rune & 0x3F));
			return true;
		case 4:
			harbol_string_add_char(str, 0xF0 | (rune >> 18));
			harbol_string_add_char(str, 0x80 | ((rune >> 12) & 0x3F));
			harbol_string_add_char(str, 0x80 | ((rune >> 6) & 0x3F));
			harbol_string_add_char(str, 0x80 | (rune & 0x3F));
			return true;
		default:
			return false;
	}
}

static NO_NULL size_t _prechecked_utf8_read(char const cstr[const restrict], int32_t *const restrict rune) {
	if( (cstr[0] & 0xF8)==0xF0 ) {
		*rune = ((cstr[0] & 0x07) << 18) | ((cstr[1] & 0x3F) << 12) | ((cstr[2] & 0x3F) << 6) | (cstr[3] & 0x3F);
		return 4;
	} else if( (cstr[0] & 0xF0)==0xE0 ) {
		*rune = ((cstr[0] & 0x0F) << 12) | ((cstr[1] & 0x3F) << 6) | (cstr[2] & 0x3F);
		return 3;
	} else if( (cstr[0] & 0xE0)==0xC0 ) {
		*rune = ((0x1F & cstr[0]) << 6) | (cstr[1] & 0x3F);
		return 2;
	}
	*rune = cstr[0];
	return 1;
}

HARBOL_EXPORT size_t read_utf8(char const cstr[const restrict static 1], size_t const cstrlen, int32_t *const restrict rune) {
	*rune = -1;
	if( cstr[0]==0 ) {
		return 0;
	}
	size_t const utf8len = get_utf8_len(cstr[0]);
	return( utf8len > cstrlen )? 0 : _prechecked_utf8_read(cstr, rune);
}

HARBOL_EXPORT int32_t read_utf8_rune(char const cstr[], size_t cstrlen) {
	if( cstr[0]==0 ) {
		return 0;
	}
	
	size_t const utf8len = get_utf8_len(cstr[0]);
	if( utf8len > cstrlen ) {
		return -1;
	}
	
	int32_t rune = 0;
	return _prechecked_utf8_read(cstr, &rune) > 0? rune : -1;
}

HARBOL_EXPORT int32_t *utf8_cstr_to_rune(char const cstr[const restrict static 1], size_t const cstr_len, size_t *const restrict rune_len) {
	size_t const rune_count = get_str_rune_len(cstr);
	int32_t *restrict runes = calloc(rune_count + 1, sizeof *runes);
	if( runes==NULL ) {
		*rune_len = 0;
		return NULL;
	}
	
	size_t
		iter_len  = 0,
		rune_iter = 0
	;
	while( cstr[iter_len] != 0 && rune_iter < rune_count ) {
		int32_t rune = 0;
		size_t const bytes_read = read_utf8(&cstr[iter_len], cstr_len - iter_len, &rune);
		if( rune==0 ) {
			break;
		}
		iter_len += bytes_read;
		runes[rune_iter] = rune;
		rune_iter++;
	}
	*rune_len = rune_count;
	runes[rune_count] = 0;
	return runes;
}

HARBOL_EXPORT int32_t *utf8_str_to_rune(struct HarbolString const *const str, size_t *const rune_len) {
	return utf8_cstr_to_rune(str->cstr, str->len, rune_len);
}

HARBOL_EXPORT struct HarbolString rune_to_utf8_str(int32_t const runes[static 1]) {
	struct HarbolString str = {0};
	for( size_t i=0; runes[i] != 0; i++ ) {
		if( !write_utf8_str(&str, runes[i]) ) {
			break;
		}
	}
	return str;
}

HARBOL_EXPORT char *rune_to_utf8_cstr(int32_t const runes[static 1], size_t *const cstr_len) {
	struct HarbolString const str = rune_to_utf8_str(runes);
	*cstr_len = str.len;
	return str.cstr;
}


HARBOL_EXPORT int32_t lex_hex_escape_char(char const str[static 1], char const **const end) {
	int32_t r = 0;
	size_t count = 0;
	if( !is_hex(*str) ) {
		r = -1;
	} else {
		for( ; *str != 0; count++ ) {
			int_fast8_t const c = *str;
			switch( c ) {
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					r = (r << 4) | (c - '0'); break;
				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
					r = (r << 4) | (c - 'a' + 10); break;
				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
					r = (r << 4) | (c - 'A' + 10); break;
				default:
					goto exit;
			}
			str++;
		}
	}
exit:;
	*end = str;
	return r;
}

HARBOL_EXPORT int32_t lex_octal_escape_char(char const str[static 1], char const **const end) {
	int32_t r = 0;
	size_t count = 0;
	if( !is_octal(*str) ) {
		r = -1;
	} else {
		for( ; *str != 0; count++ ) {
			int_fast8_t const c = *str;
			if( count > 3 ) {
				return -1; /// out of range.
			} else {
				switch( c ) {
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
						r = (r << 3) | (c - '0'); break;
					default:
						goto exit;
				}
			}
			str++;
		}
	}
exit:;
	*end = str;
	return r;
}

HARBOL_EXPORT int32_t lex_unicode_char(char const str[static 1], char const **const end, size_t const encoding) {
	int32_t r = 0;
	for( size_t i=0; i < encoding*2; i++ ) {
		int_fast8_t const c = *str;
		switch( c ) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				r = (r << 4) | (c - '0'); break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
				r = (r << 4) | (c - 'a' + 10); break;
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				r = (r << 4) | (c - 'A' + 10); break;
			default:
				goto exit;
		}
		str++;
	}
exit:;
	*end = str;
	return !is_valid_unicode(r)? -1 : r;
}

HARBOL_EXPORT int lex_c_style_hex(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		/// missing initial 0.
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_c_style_hex_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	if( *str != 'x' && *str != 'X' ) {
		/// missing following `x`.
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissingX;
		goto lex_c_style_hex_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		uflag      = 1u << 0u,
		long1      = 1u << 1u,
		long2      = 1u << 2u,
		flt_dot    = 1u << 3u,
		exponent_p = 1u << 4u, /// xxx.xpxxx
		one_hex    = 1u << 5u,
		f_suffix   = 1u << 6u,
		math_op    = 1u << 7u,
		digit_sep  = 1u << 8u
	;
	while( *str != 0 && (isalnum(*str) || *str=='.' || *str=='+' || *str=='-' || *str==DigitSep_C) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case '.':
				*is_float = true;
				if( !(lit_flags & one_hex) ) { /// missing at least one hex number before float dot.
					harbol_string_add_char(buf, chr);
					result = HarbolLexMissingHexB4dot;
					goto lex_c_style_hex_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_C ) { /// digit sep before or after dot.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearDot;
					goto lex_c_style_hex_err;
				} else {
					lit_flags |= flt_dot;
					lit_flags &= ~one_hex;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'P': case 'p':
				*is_float = true;
				if( (lit_flags & exponent_p) ) { /// too many P's.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyPs;
					goto lex_c_style_hex_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_C ) { /// digit sep before or after exponent.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearExp;
					goto lex_c_style_hex_err;
				} else {
					lit_flags |= exponent_p;
					lit_flags &= ~one_hex;
					harbol_string_add_char(buf, chr);
				}
				break;
			case '+': case '-':
				if( lit_flags & math_op ) {
					goto lex_c_style_hex_err;
				} else if( !(lit_flags & (exponent_p|flt_dot)) ) { /// bad +/- placement.
					harbol_string_add_char(buf, chr);
					result = HarbolLexBadPlusMinusPlace;
					goto lex_c_style_hex_err;
				} else if( !is_decimal(str[1]) && !(lit_flags & math_op) ) { /// no number after exponent?
					harbol_string_add_char(buf, chr);
					result = HarbolLexNoNumAfterExp;
					goto lex_c_style_hex_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= math_op;
				}
				break;
			case 'U': case 'u':
				if( lit_flags & uflag ) { /// too many U's.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyUs;
					goto lex_c_style_hex_err;
				} else if( (lit_flags & long1) && (str[1]=='L'||str[1]=='l') ) { /// U between L's.
					harbol_string_add_char(buf, chr);
					result = HarbolLexUBetweenLs;
					goto lex_c_style_hex_err;
				} else {
					lit_flags |= uflag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'L': case 'l':
				if( lit_flags & long2 ) { /// too many L's.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyLs;
					goto lex_c_style_hex_err;
				} else {
					lit_flags |= ( (lit_flags & long1)? long2 : long1 );
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
				if( (lit_flags & exponent_p) ) {
					if( !(lit_flags & one_hex) ) { /// hex float exponent with no digits?
						harbol_string_add_char(buf, chr);
						result = HarbolLexHexNoDigitsB4Exp;
						goto lex_c_style_hex_err;
					} else if( (chr=='F'||chr=='f') ) { /// invalid hex float suffix!
						harbol_string_add_char(buf, chr);
						if( lit_flags & f_suffix ) {
							result = HarbolLexFltExtraSuffix;
							goto lex_c_style_hex_err;
						} else {
							lit_flags |= f_suffix;
							break;
						}
					} else if( lit_flags & f_suffix ) { /// also bad hex float suffix.
						result = HarbolLexFltExtraSuffix;
						harbol_string_add_char(buf, chr);
						goto lex_c_style_hex_err;
					}
				}
				if( !(lit_flags & one_hex) ) {
					lit_flags |= one_hex;
				}
				
				if( lit_flags & (uflag|long1|long2) ) { /// extraneous suffix on int hex?
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntExtraSuffix;
					goto lex_c_style_hex_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags &= ~digit_sep;
				}
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if( !(lit_flags & one_hex) ) {
					lit_flags |= one_hex;
				}
				if( lit_flags & (uflag|long1|long2) ) { /// extraneous suffix on int hex?
					result = HarbolLexIntExtraSuffix;
					harbol_string_add_char(buf, chr);
					goto lex_c_style_hex_err;
				} else if( lit_flags & f_suffix ) { /// bad hex float suffix.
					harbol_string_add_char(buf, chr);
					result = HarbolLexFltExtraSuffix;
					goto lex_c_style_hex_err;
				} else {
					harbol_string_add_char(buf, chr);
				}
				lit_flags &= ~digit_sep;
				break;
			case DigitSep_C:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_c_style_hex_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			default:
				if( chr <= -1 ) {
					/// add foreign numbers.
				} else { /// invalid hex digit/glyph.
					harbol_string_add_char(buf, chr);
					result = HarbolLexBadGylph;
					goto lex_c_style_hex_err;
				}
				break;
		}
		str++;
	}
	if( (lit_flags & flt_dot) && !(lit_flags & exponent_p) ) { /// missing exponent...
		result = HarbolLexHexFltNoExp;
		goto lex_c_style_hex_err;
	} else if( !(lit_flags & one_hex) ) { /// hex float or int with no digits or bad suffix.
		result = HarbolLexHexMissingDigits;
		goto lex_c_style_hex_err;
	} else if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_c_style_hex_err;
	}
lex_c_style_hex_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_go_style_hex(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_go_style_hex_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	if( *str != 'x' && *str != 'X' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissingX;
		goto lex_go_style_hex_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		flt_dot    = 1u << 0u,
		exponent_p = 1u << 1u, /** xxx.xpxxx */
		digit_sep  = 1u << 2u,
		math_op    = 1u << 3u,
		one_hex    = 1u << 4u
	;
	while( *str != 0 && (isalnum(*str) || *str=='.' || *str=='+' || *str=='-' || *str==DigitSep_Go) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case '.':
				*is_float = true;
				if( (lit_flags & digit_sep) || str[1]==DigitSep_Go ) { /// digit sep before or after dot.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearDot;
					goto lex_go_style_hex_err;
				} else {
					lit_flags |= flt_dot;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'P': case 'p':
				*is_float = true;
				if( !(lit_flags & one_hex) ) { /// missing at least one hex number before hex exponent.
					harbol_string_add_char(buf, chr);
					result = HarbolLexHexNoDigitsB4Exp;
					goto lex_go_style_hex_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_Go ) { /// digit sep before or after exponent.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearExp;
					goto lex_go_style_hex_err;
				} else {
					lit_flags |= exponent_p;
					harbol_string_add_char(buf, chr);
				}
				break;
			case '+': case '-':
				if( lit_flags & math_op ) {
					goto lex_go_style_hex_err;
				} else if( lit_flags & (exponent_p|flt_dot) ) {
					if( !is_decimal(str[1]) ) { /// no number after exponent?
						result = HarbolLexNoNumAfterExp;
						harbol_string_add_char(buf, chr);
						goto lex_go_style_hex_err;
					} else {
						lit_flags |= math_op;
						harbol_string_add_char(buf, chr);
					}
				} else {
					goto lex_go_style_hex_err;
				}
				break;
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if( !(lit_flags & one_hex) ) {
					lit_flags |= one_hex;
				}
				harbol_string_add_char(buf, chr);
				lit_flags &= ~digit_sep;
				break;
			case DigitSep_Go:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_go_style_hex_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			default: /// invalid hex digit/glyph.
				result = HarbolLexBadGylph;
				harbol_string_add_char(buf, chr);
				goto lex_go_style_hex_err;
		}
		str++;
	}
	if( (lit_flags & flt_dot) && !(lit_flags & exponent_p) ) { /// hex float missing exponent.
		result = HarbolLexHexFltNoExp;
		goto lex_go_style_hex_err;
	} else if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_go_style_hex_err;
	}
lex_go_style_hex_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_c_style_octal(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_c_style_octal_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		uflag     = 1u,
		long1     = 1u << 1u,
		long2     = 1u << 2u,
		digit_sep = 1u << 3u
	;
	while( *str != 0 && (isalnum(*str) || *str=='.' || *str==DigitSep_C) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case DigitSep_C:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_c_style_octal_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			case '.':
				return lex_c_style_decimal(str, end, buf, is_float);
			case 'U': case 'u':
				if( lit_flags & uflag ) { /// too many Us.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyUs;
					goto lex_c_style_octal_err;
				} else if( (lit_flags & long1) && (str[1]=='L'||str[1]=='l') ) { /// U in between Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexUBetweenLs;
					goto lex_c_style_octal_err;
				} else {
					lit_flags |= uflag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'L': case 'l':
				if( lit_flags & long2 ) { /// too many Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyLs;
					goto lex_c_style_octal_err;
				} else {
					lit_flags |= ( (lit_flags & long1)? long2 : long1 );
					harbol_string_add_char(buf, chr);
				}
				break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				if( lit_flags & (uflag|long1|long2) ) { /// add more digits after the int suffix...
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntExtraSuffix;
					goto lex_c_style_octal_err;
				} else {
					harbol_string_add_char(buf, chr);
				}
				lit_flags &= ~digit_sep;
				break;
			default: /// bad digit/glyph.
				harbol_string_add_char(buf, chr);
				result = HarbolLexBadGylph;
				goto lex_c_style_octal_err;
				break;
		}
		str++;
	}
	
	if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_c_style_octal_err;
	}
lex_c_style_octal_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_go_style_octal(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_go_style_octal_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	if( *str != 'o' && *str != 'O' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissingO;
		goto lex_go_style_octal_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		digit_sep = 1u
	;
	while( *str != 0 && (isalnum(*str) || *str==DigitSep_Go) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case DigitSep_Go:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_go_style_octal_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				harbol_string_add_char(buf, chr);
				lit_flags &= ~digit_sep;
				break;
			default: /// bad digit/glyph.
				harbol_string_add_char(buf, chr);
				result = HarbolLexBadGylph;
				goto lex_go_style_octal_err;
				break;
		}
		str++;
	}
	
	if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_go_style_octal_err;
	}
lex_go_style_octal_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_c_style_binary(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_c_style_binary_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	if( *str != 'b' && *str != 'B' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissingB;
		goto lex_c_style_binary_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		uflag     = 1u << 0u,
		long1     = 1u << 1u,
		long2     = 1u << 2u,
		digit_sep = 1u << 3u
	;
	while( *str != 0 && (isalnum(*str) || *str==DigitSep_C) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case DigitSep_C:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_c_style_binary_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			case 'U': case 'u':
				if( lit_flags & uflag ) { /// too many Us.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyUs;
					goto lex_c_style_binary_err;
				} else if( (lit_flags & long1) && (str[1]=='L'||str[1]=='l') ) { /// U in between Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexUBetweenLs;
					goto lex_c_style_binary_err;
				} else {
					lit_flags |= uflag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'L': case 'l':
				if( lit_flags & long2 ) { /// too many Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyLs;
					goto lex_c_style_binary_err;
				} else {
					lit_flags |= ( (lit_flags & long1)? long2 : long1 );
					harbol_string_add_char(buf, chr);
				}
				break;
			case '0': case '1':
				if( lit_flags & (uflag|long1|long2) ) { /// add more digits after the int suffix...
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntExtraSuffix;
					goto lex_c_style_binary_err;
				} else {
					harbol_string_add_char(buf, chr);
				}
				lit_flags &= ~digit_sep;
				break;
			default: /// bad digit/glyph.
				harbol_string_add_char(buf, chr);
				result = HarbolLexBadGylph;
				goto lex_c_style_binary_err;
				break;
		}
		str++;
	}
	
	if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_c_style_binary_err;
	}
lex_c_style_binary_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_go_style_binary(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	} else if( *str != '0' ) {
		harbol_string_add_char(buf, *str++);
		result = HarbolLexMissing0;
		goto lex_go_style_binary_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	if( *str != 'b' && *str != 'B' ) {
		result = HarbolLexMissingB;
		harbol_string_add_char(buf, *str++);
		goto lex_go_style_binary_err;
	} else {
		harbol_string_add_char(buf, *str++);
	}
	
	size_t lit_flags = 0;
	size_t const
		digit_sep = 1u
	;
	while( *str != 0 && (isalnum(*str) || *str==DigitSep_Go) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case DigitSep_Go:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_go_style_binary_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			case '0': case '1':
				harbol_string_add_char(buf, chr);
				lit_flags &= ~digit_sep;
				break;
			default: /// bad digit/glyph.
				result = HarbolLexBadGylph;
				harbol_string_add_char(buf, chr);
				goto lex_go_style_binary_err;
				break;
		}
		str++;
	}
	
	if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_go_style_binary_err;
	}
lex_go_style_binary_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_c_style_decimal(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	}
	size_t lit_flags = 0;
	size_t const
		uflag       = 1u << 0u,
		long1       = 1u << 1u,
		long2       = 1u << 2u,
		flt_dot     = 1u << 3u,
		flt_f_flag  = 1u << 4u,
		flt_e_flag  = 1u << 5u,
		got_exp_num = 1u << 6u,
		math_op     = 1u << 7u,
		digit_sep   = 1u << 8u
	;
	while( *str != 0 && (isalnum(*str) || *str=='.' || *str=='+' || *str=='-' || *str==DigitSep_C) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case '.':
				if( lit_flags & flt_dot ) { /// too many float dots.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraFltDot;
					goto lex_c_style_decimal_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_C ) { /// digit sep before or after dot.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearDot;
					goto lex_c_style_decimal_err;
				} else {
					lit_flags |= flt_dot;
					harbol_string_add_char(buf, chr);
					*is_float = true;
				}
				break;
			case '-': case '+':
				if( (lit_flags & (math_op|got_exp_num)) || lit_flags==0 ) {
					goto lex_c_style_decimal_err;
				} else if( lit_flags & (flt_e_flag|flt_dot) ) {
					if( !is_decimal(str[1]) ) { /// no number after exponent?
						harbol_string_add_char(buf, chr);
						result = HarbolLexNoNumAfterExp;
						goto lex_c_style_decimal_err;
					} else {
						lit_flags |= math_op;
						harbol_string_add_char(buf, chr);
					}
				}
				break;
			case 'F': case 'f':
				if( !(lit_flags & (flt_dot|flt_e_flag)) ) { /// missing dot or exponent.
					harbol_string_add_char(buf, chr);
					result = HarbolLexMissingDotOrExp;
					goto lex_c_style_decimal_err;
				} else if( lit_flags & flt_f_flag ) { /// already have an F float suffix.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraFltSuffix;
					goto lex_c_style_decimal_err;
				} else if( (lit_flags & flt_e_flag) && !(lit_flags & got_exp_num) ) { /// f suffix but no exponent numbers after e?
					harbol_string_add_char(buf, chr);
					result = HarbolLexFltSuffixAfterExpNoDigits;
					goto lex_c_style_decimal_err;
				} else {
					lit_flags |= flt_f_flag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'E': case 'e':
				if( lit_flags & flt_e_flag ) { /// too many Es.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraExp;
					goto lex_c_style_decimal_err;
				} else if( lit_flags & flt_f_flag ) { /// bad float suffix. E should be before F.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExpAfterFltSuffix;
					goto lex_c_style_decimal_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_C ) { /// not separating numbers.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNotSepDigits;
					goto lex_c_style_decimal_err;
				} else {
					lit_flags |= flt_e_flag;
					harbol_string_add_char(buf, chr);
					*is_float = true;
				}
				break;
			case 'U': case 'u':
				if( lit_flags & uflag ) { /// too many Us.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyUs;
					goto lex_c_style_decimal_err;
				} else if( (lit_flags & long1) && (str[1]=='L'||str[1]=='l') ) { /// U in between Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexUBetweenLs;
					goto lex_c_style_decimal_err;
				} else if( lit_flags & (flt_dot|flt_f_flag|flt_e_flag) ) { /// int suffix on float literal.
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntSuffixOnFlt;
					goto lex_c_style_decimal_err;
				} else {
					lit_flags |= uflag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case 'L': case 'l':
				if( lit_flags & long2 ) { /// too many Ls.
					harbol_string_add_char(buf, chr);
					result = HarbolLexTooManyLs;
					goto lex_c_style_decimal_err;
				} else if( (lit_flags & (flt_dot|flt_f_flag|flt_e_flag)) && (lit_flags & (long2|uflag)) ) { /// int suffix on float literal.
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntSuffixOnFlt;
					goto lex_c_style_decimal_err;
				} else {
					lit_flags |= ( (lit_flags & long1)? long2 : long1 );
					harbol_string_add_char(buf, chr);
				}
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if( lit_flags & flt_e_flag ) {
					lit_flags |= got_exp_num;
				}
				if( lit_flags & (uflag|long1|long2) ) { /// numbers after int suffix.
					harbol_string_add_char(buf, chr);
					result = HarbolLexIntExtraSuffix;
					goto lex_c_style_decimal_err;
				} else if( lit_flags & flt_f_flag ) { /// numbers after float suffix.
					harbol_string_add_char(buf, chr);
					result = HarbolLexFltExtraSuffix;
					goto lex_c_style_decimal_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags &= ~digit_sep;
				}
				break;
			case DigitSep_C:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_c_style_decimal_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			default: /// bad digit/glyph.
				harbol_string_add_char(buf, chr);
				result = HarbolLexBadGylph;
				goto lex_c_style_decimal_err;
				break;
		}
		str++;
	}
	
	if( (lit_flags & flt_e_flag) && (str[-1]=='e'||str[-1]=='E') ) { /// float missing exponent!
		result = HarbolLexNoNumAfterExp;
		goto lex_c_style_decimal_err;
	} else if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_c_style_decimal_err;
	}
lex_c_style_decimal_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_go_style_decimal(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	int result = HarbolLexNoErr;
	if( *str==0 ) {
		return HarbolLexEoF;
	}
	size_t lit_flags = 0;
	size_t const
		flt_dot = 1u << 0u,
		flt_e_flag = 1u << 1u,
		got_exp_num = 1u << 2u,
		math_op = 1u << 3u,
		digit_sep = 1u << 4u
	;
	while( *str != 0 && (isalnum(*str) || *str=='.' || *str=='+' || *str=='-' || *str==DigitSep_Go) ) {
		int_fast8_t const chr = *str;
		switch( chr ) {
			case '.':
				*is_float = true;
				if( lit_flags & flt_dot ) { /// too many float dots.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraFltDot;
					goto lex_go_style_decimal_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_Go ) { /// digit sep before or after dot.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNearDot;
					goto lex_go_style_decimal_err;
				} else {
					lit_flags |= flt_dot;
					harbol_string_add_char(buf, chr);
				}
				break;
			case '-': case '+':
				if( (lit_flags & math_op) || lit_flags==0 ) {
					goto lex_go_style_decimal_err;
				} else if( lit_flags & (flt_e_flag|flt_dot) ) {
					if( !is_decimal(str[1]) ) { /// no number after exponent?
						harbol_string_add_char(buf, chr);
						result = HarbolLexNoNumAfterExp;
						goto lex_go_style_decimal_err;
					} else {
						lit_flags |= math_op;
						harbol_string_add_char(buf, chr);
					}
				}
				break;
			case 'E': case 'e':
				*is_float = true;
				if( lit_flags & flt_e_flag ) { /// too many Es.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraExp;
					goto lex_go_style_decimal_err;
				} else if( (lit_flags & digit_sep) || str[1]==DigitSep_Go ) { /// not separating numbers.
					harbol_string_add_char(buf, chr);
					result = HarbolLexDigitSepNotSepDigits;
					goto lex_go_style_decimal_err;
				} else {
					lit_flags |= flt_e_flag;
					harbol_string_add_char(buf, chr);
				}
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				if( lit_flags & flt_e_flag ) {
					lit_flags |= got_exp_num;
				}
				harbol_string_add_char(buf, chr);
				lit_flags &= ~digit_sep;
				break;
			case DigitSep_Go:
				if( lit_flags & digit_sep ) { /// too many digit seps.
					harbol_string_add_char(buf, chr);
					result = HarbolLexExtraDigitSeps;
					goto lex_go_style_decimal_err;
				} else {
					harbol_string_add_char(buf, chr);
					lit_flags |= digit_sep;
				}
				break;
			default: /// bad digit/glyph.
				harbol_string_add_char(buf, chr);
				result = HarbolLexBadGylph;
				goto lex_go_style_decimal_err;
				break;
		}
		str++;
	}
	
	if( (lit_flags & flt_e_flag) && (str[-1]=='e'||str[-1]=='E') ) { /// e mark missing exponent!
		result = HarbolLexNoNumAfterExp;
		goto lex_go_style_decimal_err;
	} else if( lit_flags & digit_sep ) { /// digit seps can only separate digits.
		result = HarbolLexDigitSepNotSepDigits;
		goto lex_go_style_decimal_err;
	}
lex_go_style_decimal_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_c_style_number(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	switch( *str ) {
		case '0': {
			switch( str[1] ) {
				case 'x': case 'X': return lex_c_style_hex(str, end, buf, is_float);
				case 'b': case 'B': return lex_c_style_binary(str, end, buf);
				case '.':           return lex_c_style_decimal(str, end, buf, is_float);
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
									return lex_c_style_octal(str, end, buf, is_float);
				default:            return lex_c_style_decimal(str, end, buf, is_float);
			}
		}
		case '.': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			return lex_c_style_decimal(str, end, buf, is_float);
		}
	}
	return HarbolLexNoErr;
}

HARBOL_EXPORT int lex_go_style_number(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool *const restrict is_float) {
	switch( *str ) {
		case '0': {
			switch( str[1] ) {
				case 'x': case 'X': return lex_go_style_hex(str, end, buf, is_float);
				case 'b': case 'B': return lex_go_style_binary(str, end, buf);
				case 'o': case 'O': return lex_go_style_octal(str, end, buf);
				default:            return lex_go_style_decimal(str, end, buf, is_float);
			}
		}
		case '.': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': {
			return lex_go_style_decimal(str, end, buf, is_float);
		}
	}
	return HarbolLexNoErr;
}

static NO_NULL int _lex_str(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool const raw) {
	int result = HarbolLexNoErr;
	for( int_fast8_t const quote = *str++; *str != quote; str++ ) {
		int_fast8_t const c = *str;
		if( c==0 ) {
			result = HarbolLexSuddenEoFStr;
			goto lex_str_err;
		} else if( c=='\\' ) {
			char const esc = *++str;
			if( raw ) {
				harbol_string_add_char(buf, c);
				harbol_string_add_char(buf, esc);
			} else {
				switch( esc ) {
					case 'a': harbol_string_add_char(buf, '\a'); break;
					case 'r': harbol_string_add_char(buf, '\r'); break;
					case 'b': harbol_string_add_char(buf, '\b'); break;
					case 't': harbol_string_add_char(buf, '\t'); break;
					case 'v': harbol_string_add_char(buf, '\v'); break;
					case 'n': harbol_string_add_char(buf, '\n'); break;
					case 'N': harbol_string_add_char(buf, '\n'); break;
					case 'f': harbol_string_add_char(buf, '\f'); break;
					case 'e': harbol_string_add_char(buf, 0x1B); break;  /// '\e' is GNU extension
					case 'x': case 'X': {
						str++;
						int32_t const h = lex_hex_escape_char(str, &str);
						if( h == -1 ) {
							result = HarbolLexBadHexChar;
							goto lex_str_err;
						} else {
							write_utf8_str(buf, h);
						}
						str--;
						break;
					}
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9': {
						int32_t const h = lex_octal_escape_char(str, &str);
						if( h == -1 ) {
							result = HarbolLexBadOctalChar;
							goto lex_str_err;
						} else {
							write_utf8_str(buf, h);
						}
						str--;
						break;
					}
					case 'u': case 'U': {
						str++;
						int32_t const h = lex_unicode_char(str, &str, esc=='u'? sizeof(int16_t) : sizeof(int32_t));
						if( h == -1 ) {
							result = HarbolLexBadUnicodeChar;
							goto lex_str_err;
						} else {
							write_utf8_str(buf, h);
						}
						str--;
						break;
					}
					default: harbol_string_add_char(buf, esc);
				}
			}
		} else {
			harbol_string_add_char(buf, c);
		}
	}
	str++;
lex_str_err:;
	*end = str;
	return result;
}

HARBOL_EXPORT int lex_c_style_str(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	return _lex_str(str, end, buf, false);
}

HARBOL_EXPORT int lex_go_style_str(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	return _lex_str(str, end, buf, *str=='`');
}

HARBOL_EXPORT char const *lex_get_err(int const err_code) {
	switch( err_code ) {
		case HarbolLexNoErr:                     return "No Lexing Error.";
		case HarbolLexEoF:                       return "Sudden End of File/Str.";
		case HarbolLexMissing0:                  return "Missing initial '0'.";
		case HarbolLexMissingX:                  return "Missing hex notation 'x'.";
		case HarbolLexMissingO:                  return "Missing octal notation 'o'.";
		case HarbolLexMissingB:                  return "Missing binary notation 'b'.";
		case HarbolLexMissingHexB4dot:           return "Missing hex digit before dot.";
		case HarbolLexTooManyPs:                 return "Too many P marks in hex literal.";
		case HarbolLexBadPlusMinusPlace:         return "Bad +/- placement.";
		case HarbolLexNoNumAfterExp:             return "Missing digits after exponent.";
		case HarbolLexTooManyUs:                 return "Too many 'U' suffixes.";
		case HarbolLexTooManyLs:                 return "Too many 'L' suffixes.";
		case HarbolLexUBetweenLs:                return "'U' suffix between 'L' suffixes.";
		case HarbolLexHexFltExpNoDigits:         return "Hex Float exponent missing digits.";
		case HarbolLexHexFltBadSuffix:           return "Hex Float with bad suffix.";
		case HarbolLexIntExtraSuffix:            return "Extraneous integer suffix.";
		case HarbolLexFltExtraSuffix:            return "Extraneous float suffix.";
		case HarbolLexBadGylph:                  return "Bad digit/gylph.";
		case HarbolLexHexMissingDigits:          return "Hex literal missing digits.";
		case HarbolLexHexNoDigitsB4Exp:          return "Missing at least one hex digit before exponent.";
		case HarbolLexDigitSepNearDot:           return "Digit Separator near dot.";
		case HarbolLexDigitSepNearExp:           return "Digit Separator near exponent mark.";
		case HarbolLexExtraDigitSeps:            return "Too many digit separators.";
		case HarbolLexHexFltNoExp:               return "Hex float with no exponent.";
		case HarbolLexDigitSepMIADigits:         return "Digit Separator without digits.";
		case HarbolLexExtraFltDot:               return "Too many float dots.";
		case HarbolLexExtraFltSuffix:            return "Extra float suffix.";
		case HarbolLexExtraExp:                  return "Extra exponent mark.";
		case HarbolLexMissingDotOrExp:           return "Missing dot/exponent mark.";
		case HarbolLexExpAfterFltSuffix:         return "Exponent mark after float suffix.";
		case HarbolLexFltSuffixAfterExpNoDigits: return "No digits between exponent mark and float suffix.";
		case HarbolLexIntSuffixOnFlt:            return "Integer suffix on a float literal.";
		case HarbolLexBadOctalChar:              return "Bad octal escape character.";
		case HarbolLexBadHexChar:                return "Bad hex escape character.";
		case HarbolLexBadUnicodeChar:            return "Bad unicode escape character.";
		case HarbolLexSuddenEoFStr:              return "Sudden EoF in string.";
		case HarbolLexDigitSepNotSepDigits:      return "Digit Separator not separating digits.";
		default:                                 return "Unknown Lex error.";
	}
}

HARBOL_EXPORT bool lex_identifier(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool checker(int32_t c)) {
	while( *str != 0 && checker(*str) ) {
		harbol_string_add_char(buf, *str++);
	}
	*end = str;
	return buf->len > 0;
}

HARBOL_EXPORT bool lex_identifier_utf8(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, bool checker(int32_t c)) {
	bool res = false;
	char const *const ending = str + strlen(str);
	while( *str != 0 ) {
		int32_t rune = 0;
		size_t const bytes = read_utf8(str, ending - str, &rune);
		if( bytes==0 ) {
			goto lex_id_u8_err;
		} else if( checker(rune) ) {
			write_utf8_str(buf, rune);
		}
		str += bytes;
	}
	res = buf->len > 0;
lex_id_u8_err:
	*end = str;
	return res;
}

HARBOL_EXPORT bool lex_c_style_identifier(char const str[static 1], char const **const end, struct HarbolString *const restrict buf) {
	if( !is_alphabetic(*str) ) {
		return false;
	}
	while( *str != 0 && is_possible_id(*str) ) {
		harbol_string_add_char(buf, *str++);
	}
	*end = str;
	return true;
}

HARBOL_EXPORT bool lex_until(char const str[static 1], char const **const end, struct HarbolString *const restrict buf, int32_t const control) {
	while( *str != 0 && *str != control ) {
		harbol_string_add_char(buf, *str++);
	}
	*end = str;
	return buf->len > 0;
}

HARBOL_EXPORT intmax_t lex_c_string_to_int(struct HarbolString const *const str, char **const end) {
	bool const is_binary = !strncmp(str->cstr, "0b", 2) || !strncmp(str->cstr, "0B", 2);
	size_t const extra = (is_binary)? 2 : 0;
	return strtoll(&str->cstr[extra], end, is_binary? 2 : 0);
}

HARBOL_EXPORT intmax_t lex_go_string_to_int(struct HarbolString const *const str, char **const end) {
	bool const is_octal  = !strncmp(str->cstr, "0o", 2) || !strncmp(str->cstr, "0O", 2);
	bool const is_binary = !strncmp(str->cstr, "0b", 2) || !strncmp(str->cstr, "0B", 2);
	size_t const extra = (is_octal || is_binary)? 2 : 0;
	return strtoll(&str->cstr[extra], end, is_octal? 8 : is_binary? 2 : 0);
}


HARBOL_EXPORT uintmax_t lex_c_string_to_uint(struct HarbolString const *const str, char **const end) {
	bool const is_binary = !strncmp(str->cstr, "0b", 2) || !strncmp(str->cstr, "0B", 2);
	size_t const extra = (is_binary)? 2 : 0;
	return strtoull(&str->cstr[extra], end, is_binary? 2 : 0);
}

HARBOL_EXPORT uintmax_t lex_go_string_to_uint(struct HarbolString const *const str, char **const end) {
	bool const is_octal  = !strncmp(str->cstr, "0o", 2) || !strncmp(str->cstr, "0O", 2);
	bool const is_binary = !strncmp(str->cstr, "0b", 2) || !strncmp(str->cstr, "0B", 2);
	size_t const extra = (is_octal || is_binary)? 2 : 0;
	return strtoull(&str->cstr[extra], end, is_octal? 8 : is_binary? 2 : 0);
}

HARBOL_EXPORT floatmax_t lex_string_to_float(struct HarbolString const *const str) {
	bool const is_hex = !strncmp(str->cstr, "0x", 2) || !strncmp(str->cstr, "0X", 2);
	floatmax_t f = 0;
	harbol_string_scan(str, is_hex? "%" SCNxfMAX "" : "%" SCNfMAX "", &f);
	return f;
}


HARBOL_EXPORT bool lex_custom_number(char const str[static 1], char const **const end, struct LexingRules const *const restrict rules, struct HarbolString *const restrict buf) {
	(void)(rules);
	bool res = false;
	char const *const ending = str + strlen(str);
	while( *str != 0 ) {
		int32_t rune = 0;
		size_t const bytes = read_utf8(str, ending - str, &rune);
		if( bytes==0 ) {
			goto lex_custom_lit_err;
		} else {
			/// TODO:
		}
		str += bytes;
	}
	
	res = buf->len > 0;
lex_custom_lit_err:;
	*end = str;
	return res;
}

HARBOL_EXPORT void lex_fix_newlines(struct HarbolString *const str, bool const replace_tabs_w_spaces) {
	harbol_string_replace_cstr(str, "\r\n", "\n", -1);
	harbol_string_replace_cstr(str, "\r",   "\n", -1);
	if( replace_tabs_w_spaces ) {
		harbol_string_replace_cstr(str, "\t", "    ", -1);
	}
}

 
HARBOL_EXPORT intmax_t convert_runes_to_base(
	int32_t const        runes[const restrict static 1],
	uint8_t const        base,
	int32_t const        numerals[const restrict static 1],
	bool *const restrict res
) {
	intmax_t value = 0;
	int sign = 1;
	for( size_t i=0; runes[i] != 0; i++ ) {
		if( runes[i]=='-' ) {
			sign = -1;
			continue;
		}
		
		size_t const idx = find_rune_in_runes(numerals, runes[i]);
		if( idx != SIZE_MAX ) {
			value = value * base + idx;
			continue;
		}
		*res = false;
		return value;
	}
	*res = true;
	return value * sign;
}

HARBOL_EXPORT uintmax_t convert_runes_to_base_uint(
	int32_t const        runes[const restrict static 1],
	uint8_t const        base,
	int32_t const        numerals[const restrict static 1],
	bool *const restrict res
) {
	uintmax_t value = 0;
	for( size_t i=0; runes[i] != 0; i++ ) {
		size_t const idx = find_rune_in_runes(numerals, runes[i]);
		if( idx != SIZE_MAX ) {
			value = value * base + idx;
			continue;
		}
		*res = false;
		return value;
	}
	*res = true;
	return value;
}

HARBOL_EXPORT int32_t *runes_from_stream(FILE *const restrict stream, size_t *const restrict rune_len) {
	size_t len = 0;
	char *restrict text = ( char* )(make_buffer_from_file(stream, &len));
	if( text==NULL ) {
		*rune_len = 0;
		return NULL;
	}
	int32_t *runes = utf8_cstr_to_rune(text, len, rune_len);
	free(text); text = NULL;
	return runes;
}
