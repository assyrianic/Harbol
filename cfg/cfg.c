#include "cfg.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


/** CFG Parser in EBNF grammar
```ebnf
keyval  = ( string | "<include>" | "<enum>" | math ) [':'] ( value | section ) [','] .
section = '{' *keyval '}' .
value   = string | number | vec | "true" | "false" | "null" | "iota" | "<FILE>" | math .
matrix  = '[' number [','] [number] [','] [number] [','] [number] ']' .
vec     = ('v' | 'c') matrix .
string  = '"' chars '"' | "'" chars "'" .
math    = "<math" expression ">" . /// See HarbolMath module for math parser's grammar.
```
 */

typedef struct {
	size_t      errc;
	intmax_t   *local_iota, *local_enum, global_iota, global_enum;
	char const *cfg_filename;
	uint32_t    curr_line;
} HarbolCfgState;


static NO_NULL bool skip_ws_and_comments(char const **strref, HarbolCfgState *const restrict parse_state) {
	if( *strref==NULL || **strref==0 ) {
		return false;
	} else {
		while( **strref != 0 && (is_whitespace(**strref) || /// white space
				**strref=='#' || (**strref=='/' && (*strref)[1]=='/') || /// single line comment
				(**strref=='/' && (*strref)[1]=='*') || /// multi-line comment
				**strref==':' || **strref==',') ) /// delimiters.
		{
			if( is_whitespace(**strref) ) {
				/*if( **strref=='\n' ) {
					parse_state->curr_line++;
				}*/
				*strref = skip_chars(*strref, is_whitespace, &parse_state->curr_line);
			} else if( **strref=='#' || (**strref=='/' && (*strref)[1]=='/') ) {
				*strref = skip_single_line_comment(*strref, &parse_state->curr_line);
				parse_state->curr_line++;
			} else if( **strref=='/' && (*strref)[1]=='*' ) {
				*strref = skip_multi_line_comment(*strref, "*/", sizeof "*/"-1, &parse_state->curr_line);
			} else if( **strref==':' || **strref==',' ) {
				(*strref)++;
			}
		}
		return **strref != 0;
	}
}

static bool NO_NULL _lex_number(char const **restrict strref, struct HarbolString *const restrict str, enum HarbolCfgType *const typeref, HarbolCfgState *const restrict parse_state) {
	if( *strref==NULL || **strref==0 ) {
		return false;
	}
	if( **strref=='-' || **strref=='+' ) {
		harbol_string_add_char(str, *(*strref)++);
	}
	if( !is_decimal(**strref) && **strref!='.' ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid initial numeric digit: '%c'\n", **strref);
		return false;
	}
	
	bool is_float = false;
	char const *end = NULL;
	int const res = lex_c_style_number(*strref, &end, str, &is_float);
	if( res > HarbolLexNoErr  ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number '%s', %s\n", str->cstr, lex_get_err(res));
		return false;
	}
	*strref = end;
	*typeref = (is_float)? HarbolCfgType_Float : HarbolCfgType_Int;
	return str->len > 0;
}

static NO_NULL bool harbol_cfg_parse_section(struct HarbolMap *cfg, char const **code_ref, HarbolCfgState *parse_state);
static NO_NULL bool harbol_cfg_parse_number(struct HarbolMap *cfg, struct HarbolString const *key, char const **code_ref, HarbolCfgState *parse_state);


static inline floatmax_t cfg_sin(floatmax_t const x)      { return sin(x); }
static inline floatmax_t cfg_cos(floatmax_t const x)      { return cos(x); }
static inline floatmax_t cfg_tan(floatmax_t const x)      { return tan(x); }
static inline floatmax_t cfg_arcsin(floatmax_t const x)   { return asin(x); }
static inline floatmax_t cfg_arccos(floatmax_t const x)   { return acos(x); }
static inline floatmax_t cfg_arctan(floatmax_t const x)   { return atan(x); }
static inline floatmax_t cfg_ln(floatmax_t const x)       { return log(x); }
static inline floatmax_t cfg_log(floatmax_t const x)      { return log10(x); }
static inline floatmax_t cfg_floor(floatmax_t const x)    { return floor(x); }
static inline floatmax_t cfg_ceil(floatmax_t const x)     { return ceil(x); }
static inline floatmax_t cfg_round(floatmax_t const x)    { return round(x); }
static inline floatmax_t cfg_fraction(floatmax_t const x) { return x - floor(x); }
static inline floatmax_t cfg_radians(floatmax_t const x)  { return x * (cfg_arccos(-1.0) / 180.0); }
static inline floatmax_t cfg_degrees(floatmax_t const x)  { return x * (180.0 / cfg_arccos(-1.0)); }

static void _harbol_cfg_math_var_func(
	char   const                    var_name[const restrict static 1],
	size_t const                    var_len,
	floatmax_t      *const restrict value,
	HarbolMathFunc **const restrict math_func,
	void            *const          data,
	size_t const                    data_len,
	bool            *const restrict is_func
) {
	(void)(var_len);
	(void)(data_len);
	HarbolCfgState const *const restrict parse_state = data;
	if( !strcmp(var_name, "IOTA") ) {
		*value = ( floatmax_t )(parse_state->global_iota);
	} else if( !strcmp(var_name, "iota") ) {
		*value = ( floatmax_t )(*parse_state->local_iota);
	} else if( !strcmp(var_name, "ENUM") ) {
		*value = ( floatmax_t )(parse_state->global_enum);
	} else if( !strcmp(var_name, "enum") ) {
		*value = ( floatmax_t )(*parse_state->local_enum);
	} else if( !strcmp(var_name, "e") ) {
		*value = exp(( floatmax_t )(1.0));
	} else if( !strcmp(var_name, "pi") ) {
		*value = acos(( floatmax_t )(-1.0));
	} else if( !strcmp(var_name, "sin") ) {
		*math_func = cfg_sin;
		*is_func   = true;
	} else if( !strcmp(var_name, "cos") ) {
		*math_func = cfg_cos;
		*is_func   = true;
	} else if( !strcmp(var_name, "tan") ) {
		*math_func = cfg_tan;
		*is_func   = true;
	} else if( !strcmp(var_name, "arcsin") ) {
		*math_func = cfg_arcsin;
		*is_func   = true;
	} else if( !strcmp(var_name, "arccos") ) {
		*math_func = cfg_arccos;
		*is_func   = true;
	} else if( !strcmp(var_name, "arctan") ) {
		*math_func = cfg_arctan;
		*is_func   = true;
	} else if( !strcmp(var_name, "ln") ) {
		*math_func = cfg_ln;
		*is_func   = true;
	} else if( !strcmp(var_name, "log") ) {
		*math_func = cfg_log;
		*is_func   = true;
	} else if( !strcmp(var_name, "floor") ) {
		*math_func = cfg_floor;
		*is_func   = true;
	} else if( !strcmp(var_name, "ceil") ) {
		*math_func = cfg_ceil;
		*is_func   = true;
	} else if( !strcmp(var_name, "round") ) {
		*math_func = cfg_round;
		*is_func   = true;
	} else if( !strcmp(var_name, "fraction") ) {
		*math_func = cfg_fraction;
		*is_func   = true;
	} else if( !strcmp(var_name, "radians") ) {
		*math_func = cfg_radians;
		*is_func   = true;
	} else if( !strcmp(var_name, "degrees") ) {
		*math_func = cfg_degrees;
		*is_func   = true;
	}
}

static NO_NULL floatmax_t _harbol_cfg_parse_inline_math(struct HarbolString *const str, HarbolCfgState *const parse_state, bool const replace_str_with_res) {
	union {
		uintmax_t  u;
		floatmax_t f;
	} const c = { -1ULL };
	char const *math_str = strstr(str->cstr, "<math");
	if( math_str==NULL ) {
		return c.f;
	}
	char const *expr_start = math_str + sizeof "<math" - 1;
	char *math_end = strchr(expr_start, '>');
	if( math_end==NULL ) {
		/// missing ending '>'. Forgot it?
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: ending '>' for math expression\n");
		return c.f;
	}
	math_end[0] = 0; /// temporarily set it as null terminator.
	floatmax_t const result = harbol_math_parse_expr(expr_start, _harbol_cfg_math_var_func, parse_state, sizeof *parse_state);
	math_end[0] = '>';
	if( replace_str_with_res ) {
		size_t const start = ( size_t )(math_str - str->cstr);
		char result_str[30] = {0};
		snprintf(&result_str[0],  sizeof result_str - 1,  "%" PRIfMAX "", result);
		harbol_string_replace_range(str, start, (math_end==NULL)? SIZE_MAX : ( size_t )(math_end - math_str), result_str);
	}
	return result;
}

static inline bool _cfgmath_end_and_newline(int32_t const c) {
	return c != '>' && c != '\n';
}

/// keyval = ( string | "<include>" | "<enum>" | math ) [':'] ( value | section ) [','] .
static bool harbol_cfg_parse_key_val(struct HarbolMap *const restrict map, char const **cfgcoderef, HarbolCfgState *const restrict parse_state) {
	if( *cfgcoderef==NULL ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "parse error", COLOR_RED, NULL, NULL, "Harbol Config Parser :: invalid config buffer!\n");
		return false;
	} else if( **cfgcoderef==0 || !skip_ws_and_comments(cfgcoderef, parse_state) ) {
		return false;
	} else if( **cfgcoderef != '"' && **cfgcoderef != '\'' ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: missing beginning quote for key '%c'\n", **cfgcoderef);
		return false;
	}
	
	struct HarbolString keystr = {0};
	int const str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, &keystr);
	if( str_res > HarbolLexNoErr ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string key '%s' %s.\n", keystr.cstr, lex_get_err(str_res));
		harbol_string_clear(&keystr);
		return false;
	} else if( harbol_string_empty(&keystr) ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: empty string key '%s'.\n", keystr.cstr);
		harbol_string_clear(&keystr);
		return false;
	} else if( harbol_map_has_key(map, keystr.cstr, keystr.len+1) ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: duplicate string key '%s'.\n", keystr.cstr);
		harbol_string_clear(&keystr);
		return false;
	}
	skip_ws_and_comments(cfgcoderef, parse_state);
	
	if( !harbol_string_cmpcstr(&keystr, "<INCLUDE>") || !harbol_string_cmpcstr(&keystr, "<include>") ) {
		if( **cfgcoderef != '"' && **cfgcoderef != '\'' ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: file for config inclusion is missing string quotes.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		
		struct HarbolString file_path = harbol_string_make(NULL, &( bool ){false});
		int const str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, &file_path);
		if( str_res > HarbolLexNoErr ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string value '%s' for key '%s' %s.\n", file_path.cstr, keystr.cstr, lex_get_err(str_res));
			harbol_string_clear(&keystr);
			return false;
		}
		
		harbol_string_clear(&keystr);
		struct HarbolMap *included_cfg = harbol_cfg_parse_file(file_path.cstr);
		if( included_cfg==NULL ) {
			/// if we failed somehow, warn, and set the value as a null.
			harbol_write_msg(NULL, stderr, parse_state->cfg_filename, "parse warning", COLOR_MAGENTA, &parse_state->curr_line, NULL, "Harbol Config Parser :: failed to include cfg file '%s'\n", file_path.cstr);
			harbol_string_clear(&file_path);
			return true;
		} else {
			struct HarbolVariant var = harbol_variant_make(&included_cfg, sizeof included_cfg, HarbolCfgType_Map, &( bool ){0});
			bool const inclusion_res = harbol_map_insert(map, file_path.cstr, file_path.len+1, &var, sizeof var);
			harbol_string_clear(&file_path);
			return inclusion_res;
		}
	} else {
		intmax_t const local_enum_value    = *parse_state->local_enum;
		intmax_t const global_enum_value   = parse_state->global_enum;
		size_t const num_local_enum_chars  = base_10_num_chars_int(local_enum_value);
		size_t const num_global_enum_chars = base_10_num_chars_int(global_enum_value);
		
		char local_enum_str[num_local_enum_chars];
		memset(&local_enum_str[0], 0, num_local_enum_chars);
		snprintf(&local_enum_str[0],  num_local_enum_chars,  "%" PRIiMAX "", local_enum_value);
		
		char global_enum_str[num_global_enum_chars];
		memset(&global_enum_str[0], 0, num_global_enum_chars);
		snprintf(&global_enum_str[0], num_global_enum_chars, "%" PRIiMAX "", global_enum_value);
		if( harbol_string_replace_cstr(&keystr, "<enum>", local_enum_str, SIZE_MAX) ) {
			++*parse_state->local_enum;
		}
		if( harbol_string_replace_cstr(&keystr, "<ENUM>", global_enum_str, SIZE_MAX) ) {
			++parse_state->global_enum;
		}
		_harbol_cfg_parse_inline_math(&keystr, parse_state, true);
	}
	
	bool res = false;
	/// it's a section!
	if( **cfgcoderef=='{' ) {
		intmax_t *const old_iota = parse_state->local_iota;
		intmax_t *const old_enum = parse_state->local_enum;
		parse_state->local_iota = &( intmax_t ){0};
		parse_state->local_enum = &( intmax_t ){0};
		
		struct HarbolMap *subsection = harbol_map_new(4);
		if( subsection==NULL ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "memory error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate subsection for key '%s'.\n", keystr.cstr);
			harbol_string_clear(&keystr);
			return false;
		}
		
		res = harbol_cfg_parse_section(subsection, cfgcoderef, parse_state);
		struct HarbolVariant var = harbol_variant_make(&subsection, sizeof subsection, HarbolCfgType_Map, &( bool ){0});
		if( !harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var) ) {
			harbol_write_msg(NULL, stderr, parse_state->cfg_filename, "memory warning", COLOR_MAGENTA, &parse_state->curr_line, NULL, "Harbol Config Parser :: some how failed to insert subsection for key '%s', destroying...\n", keystr.cstr);
			harbol_cfg_free(&subsection);
			harbol_string_clear(&keystr);
			harbol_variant_clear(&var);
		}
		parse_state->local_iota = old_iota;
		parse_state->local_enum = old_enum;
	} else if( **cfgcoderef=='"' || **cfgcoderef=='\'' ) {
		/// string value.
		struct HarbolString *str = harbol_string_new(NULL);
		if( str==NULL ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "memory error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate string value for key '%s'.\n", keystr.cstr);
			harbol_string_clear(&keystr);
			return false;
		}
		
		int const str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, str);
		if( str_res > HarbolLexNoErr ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string value '%s' for key '%s' %s.\n", str->cstr, keystr.cstr, lex_get_err(str_res));
			harbol_string_clear(&keystr);
			return false;
		}
		char *math_marker = strstr(str->cstr, "<math");
		if( math_marker != NULL ) {
			char *const math_start = math_marker + sizeof "<math"-1;
			char *const math_end = strchr(math_start, '>');
			if( math_end != NULL ) {
				*math_end = 0;
			}
			floatmax_t const expr_result = harbol_math_parse_expr(math_start, _harbol_cfg_math_var_func, parse_state, sizeof *parse_state);
			if( math_end != NULL ) {
				*math_end = '>';
			}
			char *number = sprintf_alloc("%" PRIfMAX "", expr_result);
			harbol_string_replace_range(str, math_marker - str->cstr, ( size_t )(math_end - str->cstr), number);
			free(number); number = NULL;
		}
		struct HarbolVariant var = harbol_variant_make(&str, sizeof str, HarbolCfgType_String, &( bool ){0});
		harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
		res = true;
	} else if( **cfgcoderef=='c' || **cfgcoderef=='v' ) {
		/// color or vector value!
		char const valtype = *(*cfgcoderef)++;
		skip_ws_and_comments(cfgcoderef, parse_state);
		
		if( **cfgcoderef!='[' ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: missing '[', got '%c' instead.\n", **cfgcoderef);
			harbol_string_clear(&keystr);
			return false;
		}
		(*cfgcoderef)++;
		skip_ws_and_comments(cfgcoderef, parse_state);
		
		union {
			struct HarbolVec4D vec4d;
			union HarbolColor  color;
		} matrix_value = { {0.f, 0.f, 0.f, 0.f} };
		
		size_t iterations = 0;
		while( **cfgcoderef != 0 && **cfgcoderef != ']' ) {
			struct HarbolString numstr = {0};
			enum HarbolCfgType type = HarbolCfgType_Null;
			bool const result = _lex_number(cfgcoderef, &numstr, &type, parse_state);
			if( iterations < 4 ) {
				if( valtype=='c' ) {
					matrix_value.color.array[iterations] = ( uint8_t )(strtoul(numstr.cstr, NULL, 0));
				} else {
					switch( iterations ) {
						case 0: matrix_value.vec4d.x = lex_string_to_float(&numstr); break;
						case 1: matrix_value.vec4d.y = lex_string_to_float(&numstr); break;
						case 2: matrix_value.vec4d.z = lex_string_to_float(&numstr); break;
						case 3: matrix_value.vec4d.w = lex_string_to_float(&numstr); break;
					}
				}
				iterations++;
			}
			harbol_string_clear(&numstr);
			if( !result ) {
				harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number in %s array for key '%s'.\n", valtype=='c'? "color" : "vector", keystr.cstr);
				harbol_string_clear(&keystr);
				return false;
			}
			skip_ws_and_comments(cfgcoderef, parse_state);
		}
		if( **cfgcoderef==0 ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unexpected end of file with ending ']' missing.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		(*cfgcoderef)++;
		
		struct HarbolVariant var = (valtype=='c')?
			  harbol_variant_make(&matrix_value.color, sizeof matrix_value.color, HarbolCfgType_Color, &( bool ){0})
			: harbol_variant_make(&matrix_value.vec4d, sizeof matrix_value.vec4d, HarbolCfgType_Vec4D, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='t' ) {
		/// true bool value.
		if( strncmp("true", *cfgcoderef, sizeof("true")-1) ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("true") - 1;
		struct HarbolVariant var = harbol_variant_make(&( bool ){true}, sizeof(bool), HarbolCfgType_Bool, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='f' ) {
		/// false bool value
		if( strncmp("false", *cfgcoderef, sizeof("false")-1) ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("false") - 1;
		struct HarbolVariant var = harbol_variant_make(&( bool ){false}, sizeof(bool), HarbolCfgType_Bool, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='n' ) {
		/// null value.
		if( strncmp("null", *cfgcoderef, sizeof("null")-1) ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("null") - 1;
		struct HarbolVariant var = harbol_variant_make(&( char ){0}, sizeof(char), HarbolCfgType_Null, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='I' ) {
		/// local iota value.
		if( strncmp("IOTA", *cfgcoderef, sizeof("IOTA")-1) ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("IOTA") - 1;
		struct HarbolVariant var = harbol_variant_make(&parse_state->global_iota, sizeof parse_state->global_iota, HarbolCfgType_Int, &( bool ){0});
		parse_state->global_iota++;
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='i' ) {
		/// local iota value.
		if( strncmp("iota", *cfgcoderef, sizeof("iota")-1) ) {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("iota") - 1;
		struct HarbolVariant var = harbol_variant_make(parse_state->local_iota, sizeof *parse_state->local_iota, HarbolCfgType_Int, &( bool ){0});
		++*parse_state->local_iota;
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( is_decimal(**cfgcoderef) || **cfgcoderef=='.' || **cfgcoderef=='-' || **cfgcoderef=='+' ) {
		/// numeric value.
		res = harbol_cfg_parse_number(map, &keystr, cfgcoderef, parse_state);
	} else if( **cfgcoderef=='[' ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: array bracket missing 'c' or 'v' tag.\n");
		harbol_string_clear(&keystr);
		return false;
	} else if( **cfgcoderef=='<' ) { /// control/special value.
		/// <FILE> is the name of the config file we're parsing.
		size_t const file_cstr_len = sizeof("<file>") - 1;
		if( !strncmp("<file>", *cfgcoderef, file_cstr_len) || !strncmp("<FILE>", *cfgcoderef, file_cstr_len) ) {
			struct HarbolString *str = harbol_string_new(NULL);
			if( str==NULL ) {
				harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "memory error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate string value for key '%s'.\n", keystr.cstr);
				harbol_string_clear(&keystr);
				return false;
			}
			
			harbol_string_copy_cstr(str, ( parse_state->cfg_filename==NULL )? "C-string-cfg" : parse_state->cfg_filename);
			struct HarbolVariant var = harbol_variant_make(&str, sizeof str, HarbolCfgType_String, &( bool ){0});
			res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
			*cfgcoderef += file_cstr_len;
		} else {
			harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unknown control/command '%c'.\n", (*cfgcoderef)[1]);
			res = false;
		}
	} else {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unknown character detected '%c'.\n", **cfgcoderef);
		res = false;
	}
	harbol_string_clear(&keystr);
	skip_ws_and_comments(cfgcoderef, parse_state);
	return res;
}

static bool harbol_cfg_parse_number(struct HarbolMap *const restrict map, struct HarbolString const *const key, char const **cfgcoderef, HarbolCfgState *const restrict parse_state) {
	struct HarbolString numstr = {0};
	enum HarbolCfgType type = HarbolCfgType_Null;
	if( !_lex_number(cfgcoderef, &numstr, &type, parse_state) ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number '%s'.\n", numstr.cstr);
		harbol_string_clear(&numstr);
		return false;
	}
	
	struct HarbolVariant var = {0};
	if( type==HarbolCfgType_Float ) {
		floatmax_t f = lex_string_to_float(&numstr);
		var = harbol_variant_make(&f, sizeof f, type, &( bool ){0});
	} else {
		intmax_t i = strtoll(numstr.cstr, NULL, 0);
		var = harbol_variant_make(&i, sizeof i, HarbolCfgType_Int, &( bool ){0});
	}
	harbol_string_clear(&numstr);
	return harbol_map_insert(map, key->cstr, key->len+1, &var, sizeof var);
}

/// section = '{' <keyval> '}' ;
static bool harbol_cfg_parse_section(struct HarbolMap *const restrict map, char const **cfgcoderef, HarbolCfgState *const restrict parse_state) {
	if( **cfgcoderef!='{' ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: missing '{' but got '%c' for section.\n", **cfgcoderef);
		return false;
	}
	(*cfgcoderef)++;
	skip_ws_and_comments(cfgcoderef, parse_state);
	
	while( **cfgcoderef != 0 && **cfgcoderef != '}' ) {
		bool const res = harbol_cfg_parse_key_val(map, cfgcoderef, parse_state);
		if( !res ) {
			return false;
		}
	}
	
	if( **cfgcoderef==0 ) {
		harbol_write_msg(&parse_state->errc, stderr, parse_state->cfg_filename, "syntax error", COLOR_RED, &parse_state->curr_line, NULL, "Harbol Config Parser :: unexpected end of file with missing '}' for section.\n");
		return false;
	}
	(*cfgcoderef)++;
	return true;
}

static struct HarbolMap *_harbol_cfg_parse(char const cfgcode[static 1], HarbolCfgState *const restrict parse_state) {
	parse_state->curr_line = 1;
	char const *iter = cfgcode;
	struct HarbolMap *objs = harbol_map_new(8);
	if( objs==NULL ) {
		return NULL;
	}
	parse_state->local_iota = &( intmax_t ){0};
	parse_state->local_enum = &( intmax_t ){0};
	while( harbol_cfg_parse_key_val(objs, &iter, parse_state) );
	return objs;
}

HARBOL_EXPORT struct HarbolMap *harbol_cfg_parse_file(char const filename[static 1]) {
	HarbolCfgState parse_state = {0};
	FILE *restrict cfgfile = fopen(filename, "r");
	if( cfgfile==NULL ) {
		harbol_write_msg(&parse_state.errc, stderr, NULL, "parse error", COLOR_RED, NULL, NULL, "Harbol Config Parser :: unable to find file '%s'.\n", filename);
		return NULL;
	}
	
	struct HarbolString cfg = {0};
	bool const read_result = harbol_string_read_from_file(&cfg, cfgfile);
	fclose(cfgfile); cfgfile = NULL;
	if( !read_result ) {
		harbol_write_msg(&parse_state.errc, stderr, NULL, "parse error", COLOR_RED, NULL, NULL, "Harbol Config Parser :: failed to read file '%s' into string.\n", filename);
		return NULL;
	}
	/// fix up new lines and tabs.
	lex_fix_newlines(&cfg, true);
	parse_state.cfg_filename = filename;
	struct HarbolMap *const restrict objs = _harbol_cfg_parse(cfg.cstr, &parse_state);
	harbol_string_clear(&cfg);
	return objs;
}


HARBOL_EXPORT struct HarbolMap *harbol_cfg_parse_cstr(char const cfgcode[static 1]) {
	HarbolCfgState parse_state = {0};
	return _harbol_cfg_parse(cfgcode, &parse_state);
}


union ConfigVal {
	uint8_t              *restrict data;
	struct HarbolMap    **restrict section;
	struct HarbolString **restrict str;
	floatmax_t           *restrict f;
	intmax_t             *restrict i;
	bool                 *restrict b;
	union HarbolColor    *restrict c;
	struct HarbolVec4D   *restrict v;
};

static void _harbol_cfgkey_clear(struct HarbolVariant *const var) {
	union ConfigVal cv = {var->data};
	switch( var->tag ) {
		case HarbolCfgType_Map:       harbol_cfg_free(cv.section); break;
		case HarbolCfgType_String:    harbol_string_free(cv.str);  break;
		default:                      break;
	}
	harbol_variant_clear(var);
}

HARBOL_EXPORT void harbol_cfg_free(struct HarbolMap **const cfg_ref) {
	if( *cfg_ref==NULL )
		return;

	struct HarbolMap *const cfg = *cfg_ref;
	for( size_t i=0; i < cfg->len; i++ ) {
		_harbol_cfgkey_clear(( struct HarbolVariant* )(cfg->datum[i]));
	}
	harbol_map_free(cfg_ref);
}

static NO_NULL void _concat_tabs(struct HarbolString *const str, size_t const tabs) {
	for( size_t i=0; i < tabs; i++ ) {
		harbol_string_add_cstr(str, "\t");
	}
}

HARBOL_EXPORT struct HarbolString harbol_cfg_to_str(struct HarbolMap const *const map) {
	static size_t tabs = 0;
	struct HarbolString cfg_str = harbol_string_make(NULL, &( bool ){false});
	struct HarbolString *str = &cfg_str;
	for( size_t i=0; i < map->len; i++ ) {
		struct HarbolVariant const *const var = ( struct HarbolVariant const* )(map->datum[i]);
		union ConfigVal const cv = { var->data };
		
		_concat_tabs(str, tabs);
		harbol_string_format(str, false, "\"%s\": ", ( char const* )(map->keys[i]));
		switch( var->tag ) {
			case HarbolCfgType_Null:
				harbol_string_add_cstr(str, "null\n");
				break;
			case HarbolCfgType_Map: {
				harbol_string_add_cstr(str, "{\n");
				tabs++;
				struct HarbolString inner_str = harbol_cfg_to_str(*cv.section);
				if( !harbol_string_empty(&inner_str) ) {
					harbol_string_add_str(str, &inner_str);
				}
				harbol_string_clear(&inner_str);
				_concat_tabs(str, --tabs);
				harbol_string_add_cstr(str, "}\n");
				break;
			}
			case HarbolCfgType_String:
				harbol_string_format(str, false, "\"%s\"\n", (*cv.str)->cstr);
				break;
			case HarbolCfgType_Float:
				harbol_string_format(str, false, "%" PRIfMAX "\n", *cv.f);
				break;
			case HarbolCfgType_Int:
				harbol_string_format(str, false, "%" PRIiMAX "\n", *cv.i);
				break;
			case HarbolCfgType_Bool:
				harbol_string_add_cstr(str, *cv.b? "true\n" : "false\n");
				break;
			case HarbolCfgType_Color:
				harbol_string_format(str, false, "c[ %u, %u, %u, %u ]\n", cv.c->bytes.r, cv.c->bytes.g, cv.c->bytes.b, cv.c->bytes.a);
				break;
			case HarbolCfgType_Vec4D:
				harbol_string_format(str, false, "v[ %" PRIf32 ", %" PRIf32 ", %" PRIf32 ", %" PRIf32 " ]\n", cv.v->x, cv.v->y, cv.v->z, cv.v->w);
				break;
		}
	}
	return cfg_str;
}

static NO_NULL bool harbol_cfg_parse_target_path(char const key[static 1], struct HarbolString *const restrict str) {
	/// parse something like: "root.section1.section2.section3.\\.dotsection"
	char const *iter = key;
	iter += strlen(key) - 1;
	while( iter != key ) {
		if( *iter=='.' ) {
			if( iter[-1]=='\\' ) {
				iter--;
			} else {
				iter++;
				break;
			}
		} else {
			iter--;
		}
	}
	
	/// now we save the target section and then use the resulting string.
	while( *iter != 0 ) {
		if( *iter=='\\' ) {
			iter++;
			continue;
		}
		harbol_string_add_char(str, *iter++);
	}
	return str->len > 0;
}

static NO_NULL struct HarbolVariant *_get_var(struct HarbolMap const *const cfgmap, char const key[const]) {
	/// first check if we're getting a singular value OR we iterate through a sectional path.
	char const *dot = strchr(key, '.');
	
	/// Patch: dot and escaped dot glitching out the hashmap hashing...
	if( dot==NULL || (dot > key && (dot[-1] == '\\')) ) {
		struct HarbolVariant *const restrict var = harbol_map_key_get(cfgmap, key, strlen(key)+1);
		return( var==NULL || var->tag==HarbolCfgType_Null )? NULL : var;
	}
	
	/// ok, not a singular value, iterate to the specific map section then.
	/// parse the target key first.
	char const *iter = key;
	struct HarbolString
		sectionstr = {0},
		targetstr = {0}
	;
	
	harbol_cfg_parse_target_path(key, &targetstr);
	struct HarbolMap const *itermap = cfgmap;
	struct HarbolVariant *restrict var = NULL;
	while( itermap != NULL ) {
		harbol_string_clear(&sectionstr);
		/// Patch: allow keys to use dot without interfering with dot path.
		while( *iter != 0 ) {
			if( *iter=='\\' && iter[1] != 0 && iter[1]=='.' ) {
				iter++;
				harbol_string_add_char(&sectionstr, *iter++);
			} else if( *iter=='.' ) {
				iter++;
				break;
			} else {
				harbol_string_add_char(&sectionstr, *iter++);
			}
		}
		if( harbol_string_empty(&sectionstr) ) {
			break;
		}
		var = harbol_map_key_get(itermap, sectionstr.cstr, sectionstr.len+1);
		if( var==NULL || !harbol_string_cmpstr(&sectionstr, &targetstr) ) {
			break;
		} else if( var->tag==HarbolCfgType_Map ) {
			itermap = *( struct HarbolMap const** )(var->data);
		}
	}
	harbol_string_clear(&sectionstr);
	harbol_string_clear(&targetstr);
	return var;
}

HARBOL_EXPORT struct HarbolMap *harbol_cfg_get_section(struct HarbolMap const *const restrict cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	//printf("harbol_cfg_get_section :: var->tag==HarbolCfgType_Map: %u\n", var != NULL? var->tag==HarbolCfgType_Map : -1);
	return( var==NULL || var->tag != HarbolCfgType_Map )? NULL : *( struct HarbolMap** )(var->data);
}

HARBOL_EXPORT char *harbol_cfg_get_cstr(struct HarbolMap const *const cfgmap, char const key[static 1], size_t *const len) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	if( var==NULL || var->tag != HarbolCfgType_String ) {
		return NULL;
	} else {
		struct HarbolString const *const str = *( struct HarbolString** )(var->data);
		*len = str->len;
		return str->cstr;
	}
}

HARBOL_EXPORT struct HarbolString *harbol_cfg_get_str(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_String )? NULL : *( struct HarbolString** )(var->data);
}

HARBOL_EXPORT floatmax_t *harbol_cfg_get_float(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Float )? NULL : ( floatmax_t* )(var->data);
}

HARBOL_EXPORT intmax_t *harbol_cfg_get_int(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Int )? NULL : ( intmax_t* )(var->data);
}

HARBOL_EXPORT bool *harbol_cfg_get_bool(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Bool )? NULL : ( bool* )(var->data);
}

HARBOL_EXPORT union HarbolColor *harbol_cfg_get_color(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Color )? NULL : ( union HarbolColor* )(var->data);
}


HARBOL_EXPORT struct HarbolVec4D *harbol_cfg_get_vec4D(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Vec4D )? NULL : ( struct HarbolVec4D* )(var->data);
}

HARBOL_EXPORT enum HarbolCfgType harbol_cfg_get_type(struct HarbolMap const *const cfgmap, char const key[static 1]) {
	struct HarbolVariant const *const var = _get_var(cfgmap, key);
	return( var==NULL )? HarbolCfgType_Invalid : var->tag;
}

HARBOL_EXPORT bool harbol_cfg_set_str(struct HarbolMap *const restrict cfgmap, char const keypath[restrict static 1], struct HarbolString const str, bool const override_convert) {
	return harbol_cfg_set_cstr(cfgmap, keypath, str.cstr, override_convert);
}

HARBOL_EXPORT bool harbol_cfg_set_cstr(struct HarbolMap *const restrict cfgmap, char const key[static 1], char const cstr[static 1], bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_String ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			struct HarbolString *str = harbol_string_new(cstr);
			*var = harbol_variant_make(&str, sizeof str, HarbolCfgType_String, &( bool ){0});
			return true;
		}
		return false;
	}
	harbol_string_copy_cstr(*( struct HarbolString** )(var->data), cstr);
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_float(struct HarbolMap *const restrict cfgmap, char const key[static 1], floatmax_t const val, bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_Float ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			*var = harbol_variant_make(&val, sizeof val, HarbolCfgType_Float, &( bool ){0});
			return true;
		}
		return false;
	}
	*( floatmax_t* )(var->data) = val;
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_int(struct HarbolMap *const restrict cfgmap, char const key[static 1], intmax_t const val, bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_Int ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			*var = harbol_variant_make(&val, sizeof val, HarbolCfgType_Int, &( bool ){0});
			return true;
		}
		return false;
	}
	*( intmax_t* )(var->data) = val;
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_bool(struct HarbolMap *const restrict cfgmap, char const key[static 1], bool const val, bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_Bool ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			*var = harbol_variant_make(&val, sizeof val, HarbolCfgType_Bool, &( bool ){0});
			return true;
		}
		return false;
	}
	*( bool* )(var->data) = val;
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_color(struct HarbolMap *const restrict cfgmap, char const key[static 1], union HarbolColor const val, bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_Color ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			*var = harbol_variant_make(&val, sizeof val, HarbolCfgType_Color, &( bool ){0});
			return true;
		}
		return false;
	}
	*( union HarbolColor* )(var->data) = val;
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_vec4D(struct HarbolMap *const restrict cfgmap, char const key[static 1], struct HarbolVec4D const val, bool const override_convert) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	} else if( var->tag != HarbolCfgType_Vec4D ) {
		if( override_convert ) {
			_harbol_cfgkey_clear(var);
			*var = harbol_variant_make(&val, sizeof val, HarbolCfgType_Vec4D, &( bool ){0});
			return true;
		}
		return false;
	}
	
	*( struct HarbolVec4D* )(var->data) = val;
	return true;
}

HARBOL_EXPORT bool harbol_cfg_set_to_null(struct HarbolMap *const restrict cfgmap, char const key[static 1]) {
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	}
	
	_harbol_cfgkey_clear(var);
	*var = harbol_variant_make(&( char ){0}, 1, HarbolCfgType_Null, &( bool ){0});
	return true;
}

static NO_NULL void _write_tabs(FILE *const file, size_t const tabs) {
	for( size_t i=0; i < tabs; i++ ) {
		fputs("\t", file);
	}
}

static NO_NULL bool _harbol_cfg_build_file(struct HarbolMap const *const map, FILE *const file, size_t const tabs) {
	for( size_t i=0; i < map->len; i++ ) {
		struct HarbolVariant const *const v = ( struct HarbolVariant const* )(map->datum[i]);
		int32_t const type = v->tag;
		_write_tabs(file, tabs);
		/// using double pointer iterators as we need the key.
		fprintf(file, "\"%s\": ", ( char const* )(map->keys[i]));
		
		union ConfigVal const cv = { v->data };
		switch( type ) {
			case HarbolCfgType_Null:
				fputs("null\n", file); break;
			case HarbolCfgType_Map:
				fputs("{\n", file);
				_harbol_cfg_build_file(*cv.section, file, tabs+1);
				_write_tabs(file, tabs);
				fputs("}\n", file);
				break;
			
			case HarbolCfgType_String: fprintf(file, "\"%s\"\n", (*cv.str)->cstr);       break;
			case HarbolCfgType_Float:  fprintf(file, "%" PRIfMAX "\n", *cv.f);           break;
			case HarbolCfgType_Int:    fprintf(file, "%" PRIiMAX "\n", *cv.i);           break;
			case HarbolCfgType_Bool:   fprintf(file, "%s\n", (*cv.b)? "true" : "false"); break;
			case HarbolCfgType_Color:
				fprintf(file, "c[ %u, %u, %u, %u ]\n", cv.c->bytes.r, cv.c->bytes.g, cv.c->bytes.b, cv.c->bytes.a); break;
			case HarbolCfgType_Vec4D:
				fprintf(file, "v[ %" PRIf32 ", %" PRIf32 ", %" PRIf32 ", %" PRIf32 " ]\n", cv.v->x, cv.v->y, cv.v->z, cv.v->w); break;
		}
	}
	return true;
}

HARBOL_EXPORT bool harbol_cfg_build_file(struct HarbolMap const *const cfg, char const filename[static 1], bool const overwrite) {
	FILE *restrict cfgfile = fopen(filename, overwrite? "w+" : "a+");
	if( cfgfile==NULL ) {
		fputs("harbol_cfg_build_file :: unable to create file.\n", stderr);
		return false;
	}
	bool const result = _harbol_cfg_build_file(cfg, cfgfile, 0);
	fclose(cfgfile); cfgfile=NULL;
	return result;
}


HARBOL_EXPORT floatmax_t harbol_cfg_calc_math(struct HarbolMap const *const restrict cfg, char const key[const restrict static 1], HarbolMathVarFunc *const var_func, void *const restrict data, size_t const data_len) {
	struct HarbolString *const restrict str = harbol_cfg_get_str(cfg, key);
	if( str==NULL ) {
		union {
			uintmax_t const  u;
			floatmax_t const f;
		} const c = { -1ULL };
		return c.f;
	}
	return harbol_math_parse_expr(str->cstr, var_func==NULL? _harbol_cfg_math_var_func : var_func, data, data_len);
}
