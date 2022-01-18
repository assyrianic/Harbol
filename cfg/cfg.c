#include "cfg.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


/** CFG Parser in EBNF grammar:
 * keyval = <string> [':'] (<value>|<section>) [','] ;
 * section = '{' *<keyval> '}' ;
 * value = <string> | <number> | <vec> | "true" | "false" | "null" | "iota" | "<FILE>" ;
 * matrix = '[' <number> [','] [<number>] [','] [<number>] [','] [<number>] ']' ;
 * vec = ('v' | 'c') <matrix> ;
 * string = '"' chars '"' | "'" chars "'" ;
 */

typedef struct {
	size_t      errc, curr_line;
	intmax_t   *local_iota, *local_enum, global_iota, global_enum;
	const char *cfg_filename;
} HarbolCfgState;


static NO_NULL bool skip_ws_and_comments(const char **strref, HarbolCfgState *const restrict parse_state)
{
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

static bool NO_NULL _lex_number(const char **restrict strref, struct HarbolString *const restrict str, enum HarbolCfgType *const typeref, HarbolCfgState *const restrict parse_state)
{
	if( *strref==NULL || **strref==0 )
		return false;
	
	if( **strref=='-' || **strref=='+' )
		harbol_string_add_char(str, *(*strref)++);
	
	if( !is_decimal(**strref) && **strref!='.' ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid initial numeric digit: '%c'\n", **strref);
		return false;
	}
	
	bool is_float = false;
	const char *end = NULL;
	const int res = lex_c_style_number(*strref, &end, str, &is_float);
	if( res > HarbolLexNoErr  ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number '%s', %s\n", str->cstr, lex_get_err(res));
		return false;
	}
	*strref = end;
	*typeref = (is_float) ? HarbolCfgType_Float : HarbolCfgType_Int;
	return str->len > 0;
}

static NO_NULL bool harbol_cfg_parse_section(struct HarbolMap *, const char **, HarbolCfgState *);
static NO_NULL bool harbol_cfg_parse_number(struct HarbolMap *, const struct HarbolString *, const char **, HarbolCfgState *);

/// keyval = <string> [':'] (<value>|<section>) [','] ;
static bool harbol_cfg_parse_key_val(struct HarbolMap *const restrict map, const char **cfgcoderef, HarbolCfgState *const restrict parse_state)
{
	if( *cfgcoderef==NULL ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "parse error", NULL, NULL, "Harbol Config Parser :: invalid config buffer!\n");
		return false;
	} else if( **cfgcoderef==0 || !skip_ws_and_comments(cfgcoderef, parse_state) ) {
		return false;
	} else if( **cfgcoderef != '"' && **cfgcoderef != '\'' ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: missing beginning quote for key '%c'\n", **cfgcoderef);
		return false;
	}
	
	struct HarbolString keystr = {0};
	const int str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, &keystr);
	if( str_res > HarbolLexNoErr ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string key '%s' %s.\n", keystr.cstr, lex_get_err(str_res));
		harbol_string_clear(&keystr);
		return false;
	} else if( harbol_string_empty(&keystr) ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: empty string key '%s'.\n", keystr.cstr);
		harbol_string_clear(&keystr);
		return false;
	} else if( harbol_map_has_key(map, keystr.cstr, keystr.len+1) ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: duplicate string key '%s'.\n", keystr.cstr);
		harbol_string_clear(&keystr);
		return false;
	}
	skip_ws_and_comments(cfgcoderef, parse_state);
	
	if( !harbol_string_cmpcstr(&keystr, "<enum>") ) {
		harbol_string_format(&keystr, true, "%" PRIiMAX "", *parse_state->local_enum);
		++*parse_state->local_enum;
	} else if( !harbol_string_cmpcstr(&keystr, "<ENUM>") ) {
		harbol_string_format(&keystr, true, "%" PRIiMAX "", parse_state->global_enum);
		++parse_state->global_enum;
	} else if( !harbol_string_cmpcstr(&keystr, "<INCLUDE>") || !harbol_string_cmpcstr(&keystr, "<include>") ) {
		if( **cfgcoderef != '"' && **cfgcoderef != '\'' ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: file for config inclusion is missing string quotes.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		
		struct HarbolString file_path = harbol_string_make(NULL, &( bool ){false});
		const int str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, &file_path);
		if( str_res > HarbolLexNoErr ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string value '%s' for key '%s' %s.\n", file_path.cstr, keystr.cstr, lex_get_err(str_res));
			harbol_string_clear(&keystr);
			return false;
		}
		
		harbol_string_clear(&keystr);
		struct HarbolMap *included_cfg = harbol_cfg_parse_file(file_path.cstr);
		if( included_cfg==NULL ) {
			/// if we failed somehow, warn, and set the value as a null.
			harbol_warn_msg(NULL, parse_state->cfg_filename, "parse warning", &parse_state->curr_line, NULL, "Harbol Config Parser :: failed to include cfg file '%s'\n", file_path.cstr);
			harbol_string_clear(&file_path);
			return true;
		} else {
			struct HarbolVariant var = harbol_variant_make(&included_cfg, sizeof included_cfg, HarbolCfgType_Map, &( bool ){0});
			const bool inclusion_res = harbol_map_insert(map, file_path.cstr, file_path.len+1, &var, sizeof var);
			harbol_string_clear(&file_path);
			return inclusion_res;
		}
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
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "memory error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate subsection for key '%s'.\n", keystr.cstr);
			harbol_string_clear(&keystr);
			return false;
		}
		
		res = harbol_cfg_parse_section(subsection, cfgcoderef, parse_state);
		struct HarbolVariant var = harbol_variant_make(&subsection, sizeof subsection, HarbolCfgType_Map, &( bool ){0});
		if( !harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var) ) {
			harbol_warn_msg(NULL, parse_state->cfg_filename, "memory warning", &parse_state->curr_line, NULL, "Harbol Config Parser :: some how failed to insert subsection for key '%s', destroying...\n", keystr.cstr);
			harbol_cfg_free(&subsection);
			harbol_string_clear(&keystr);
			harbol_variant_clear(&var);
		}
		parse_state->local_iota = old_iota;
		parse_state->local_enum = old_enum;
	} else if( **cfgcoderef=='"'||**cfgcoderef=='\'' ) {
		/// string value.
		struct HarbolString *str = harbol_string_new(NULL);
		if( str==NULL ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "memory error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate string value for key '%s'.\n", keystr.cstr);
			harbol_string_clear(&keystr);
			return false;
		}
		
		const int str_res = lex_c_style_str(*cfgcoderef, cfgcoderef, str);
		if( str_res > HarbolLexNoErr ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid string value '%s' for key '%s' %s.\n", str->cstr, keystr.cstr, lex_get_err(str_res));
			harbol_string_clear(&keystr);
			return false;
		}
		struct HarbolVariant var = harbol_variant_make(&str, sizeof str, HarbolCfgType_String, &( bool ){0});
		harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
		res = true;
	} else if( **cfgcoderef=='c' || **cfgcoderef=='v' ) {
		/// color or vector value!
		const char valtype = *(*cfgcoderef)++;
		skip_ws_and_comments(cfgcoderef, parse_state);
		
		if( **cfgcoderef!='[' ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: missing '[', got '%c' instead.\n", **cfgcoderef);
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
			const bool result = _lex_number(cfgcoderef, &numstr, &type, parse_state);
			if( iterations<4 ) {
				if( valtype=='c' ) {
					switch( iterations ) {
						case 0: matrix_value.color.bytes.r = ( uint8_t )(strtoul(numstr.cstr, NULL, 0)); break;
						case 1: matrix_value.color.bytes.g = ( uint8_t )(strtoul(numstr.cstr, NULL, 0)); break;
						case 2: matrix_value.color.bytes.b = ( uint8_t )(strtoul(numstr.cstr, NULL, 0)); break;
						case 3: matrix_value.color.bytes.a = ( uint8_t )(strtoul(numstr.cstr, NULL, 0)); break;
					}
					iterations++;
				} else {
					switch( iterations ) {
						case 0:
							matrix_value.vec4d.x = lex_string_to_float(&numstr);
							break;
						case 1:
							matrix_value.vec4d.y = lex_string_to_float(&numstr);
							break;
						case 2:
							matrix_value.vec4d.z = lex_string_to_float(&numstr);
							break;
						case 3:
							matrix_value.vec4d.w = lex_string_to_float(&numstr);
							break;
					}
					iterations++;
				}
			}
			harbol_string_clear(&numstr);
			if( !result ) {
				harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number in %s array for key '%s'.\n", valtype=='c' ? "color" : "vector", keystr.cstr);
				harbol_string_clear(&keystr);
				return false;
			}
			skip_ws_and_comments(cfgcoderef, parse_state);
		}
		if( **cfgcoderef==0 ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unexpected end of file with ending ']' missing.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		(*cfgcoderef)++;
		
		struct HarbolVariant var = (valtype=='c')
			? harbol_variant_make(&matrix_value.color, sizeof matrix_value.color, HarbolCfgType_Color, &( bool ){0})
			: harbol_variant_make(&matrix_value.vec4d, sizeof matrix_value.vec4d, HarbolCfgType_Vec4D, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='t' ) {
		/// true bool value.
		if( strncmp("true", *cfgcoderef, sizeof("true")-1) ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("true") - 1;
		struct HarbolVariant var = harbol_variant_make(&( bool ){true}, sizeof(bool), HarbolCfgType_Bool, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='f' ) {
		/// false bool value
		if( strncmp("false", *cfgcoderef, sizeof("false")-1) ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("false") - 1;
		struct HarbolVariant var = harbol_variant_make(&( bool ){false}, sizeof(bool), HarbolCfgType_Bool, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='n' ) {
		/// null value.
		if( strncmp("null", *cfgcoderef, sizeof("null")-1) ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
			harbol_string_clear(&keystr);
			return false;
		}
		*cfgcoderef += sizeof("null") - 1;
		struct HarbolVariant var = harbol_variant_make(&( char ){0}, sizeof(char), HarbolCfgType_Null, &( bool ){0});
		res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
	} else if( **cfgcoderef=='I' ) {
		/// local iota value.
		if( strncmp("IOTA", *cfgcoderef, sizeof("IOTA")-1) ) {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
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
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid keyword value, only 'true', 'false', 'null', 'iota', and 'IOTA' are allowed.\n");
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
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: array bracket missing 'c' or 'v' tag.\n");
		harbol_string_clear(&keystr);
		return false;
	} else if( **cfgcoderef=='<' ) { /// control/special value.
		/// <FILE> is the name of the config file we're parsing.
		const size_t cstr_len = sizeof("<file>") - 1;
		if( !strncmp("<file>", *cfgcoderef, cstr_len) || !strncmp("<FILE>", *cfgcoderef, cstr_len) ) {
			struct HarbolString *str = harbol_string_new(NULL);
			if( str==NULL ) {
				harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "memory error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unable to allocate string value for key '%s'.\n", keystr.cstr);
				harbol_string_clear(&keystr);
				return false;
			}
			
			harbol_string_copy_cstr(str, ( parse_state->cfg_filename==NULL )? "C-string-cfg" : parse_state->cfg_filename);
			struct HarbolVariant var = harbol_variant_make(&str, sizeof str, HarbolCfgType_String, &( bool ){0});
			res = harbol_map_insert(map, keystr.cstr, keystr.len+1, &var, sizeof var);
			*cfgcoderef += cstr_len;
		} else {
			harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unknown control/command '%c'.\n", (*cfgcoderef)[1]);
			res = false;
		}
	} else {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unknown character detected '%c'.\n", **cfgcoderef);
		res = false;
	}
	harbol_string_clear(&keystr);
	skip_ws_and_comments(cfgcoderef, parse_state);
	return res;
}

static bool harbol_cfg_parse_number(struct HarbolMap *const restrict map, const struct HarbolString *const key, const char **cfgcoderef, HarbolCfgState *const restrict parse_state)
{
	struct HarbolString numstr = {0};
	enum HarbolCfgType type = HarbolCfgType_Null;
	if( !_lex_number(cfgcoderef, &numstr, &type, parse_state) ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: invalid number '%s'.\n", numstr.cstr);
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
static bool harbol_cfg_parse_section(struct HarbolMap *const restrict map, const char **cfgcoderef, HarbolCfgState *const restrict parse_state)
{
	if( **cfgcoderef!='{' ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: missing '{' but got '%c' for section.\n", **cfgcoderef);
		return false;
	}
	(*cfgcoderef)++;
	skip_ws_and_comments(cfgcoderef, parse_state);
	
	while( **cfgcoderef != 0 && **cfgcoderef != '}' ) {
		const bool res = harbol_cfg_parse_key_val(map, cfgcoderef, parse_state);
		if( !res ) {
			return false;
		}
	}
	
	if( **cfgcoderef==0 ) {
		harbol_err_msg(&parse_state->errc, parse_state->cfg_filename, "syntax error", &parse_state->curr_line, NULL, "Harbol Config Parser :: unexpected end of file with missing '}' for section.\n");
		return false;
	}
	(*cfgcoderef)++;
	return true;
}

static struct HarbolMap *_harbol_cfg_parse(const char cfgcode[static 1], HarbolCfgState *const restrict parse_state) {
	parse_state->curr_line = 1;
	const char *iter = cfgcode;
	struct HarbolMap *objs = harbol_map_new(8);
	if( objs==NULL ) {
		return NULL;
	}
	parse_state->local_iota = &( intmax_t ){0};
	parse_state->local_enum = &( intmax_t ){0};
	while( harbol_cfg_parse_key_val(objs, &iter, parse_state) );
	return objs;
}

HARBOL_EXPORT struct HarbolMap *harbol_cfg_parse_file(const char filename[static 1])
{
	HarbolCfgState parse_state = {0};
	FILE *restrict cfgfile = fopen(filename, "r");
	if( cfgfile==NULL ) {
		harbol_err_msg(&parse_state.errc, NULL, "parse error", NULL, NULL, "Harbol Config Parser :: unable to find file '%s'.\n", filename);
		return NULL;
	}
	
	struct HarbolString cfg = {0};
	const bool read_result = harbol_string_read_from_file(&cfg, cfgfile);
	fclose(cfgfile); cfgfile = NULL;
	if( !read_result ) {
		harbol_err_msg(&parse_state.errc, NULL, "parse error", NULL, NULL, "Harbol Config Parser :: failed to read file '%s' into string.\n", filename);
		return NULL;
	}
	
	parse_state.cfg_filename = filename;
	struct HarbolMap *const restrict objs = _harbol_cfg_parse(cfg.cstr, &parse_state);
	harbol_string_clear(&cfg);
	return objs;
}


HARBOL_EXPORT struct HarbolMap *harbol_cfg_parse_cstr(const char cfgcode[static 1])
{
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

static void _harbol_cfgkey_clear(struct HarbolVariant *const var)
{
	union ConfigVal cv = {var->data};
	switch( var->tag ) {
		case HarbolCfgType_Map:       harbol_cfg_free(cv.section); break;
		case HarbolCfgType_String:    harbol_string_free(cv.str); break;
		default:                      break;
	}
	harbol_variant_clear(var);
}

HARBOL_EXPORT void harbol_cfg_free(struct HarbolMap **const cfg_ref)
{
	if( *cfg_ref==NULL )
		return;

	struct HarbolMap *const cfg = *cfg_ref;
	for( size_t i=0; i<cfg->len; i++ ) {
		_harbol_cfgkey_clear(( struct HarbolVariant* )(cfg->datum[i]));
	}
	harbol_map_free(cfg_ref);
}

static NO_NULL void _concat_tabs(struct HarbolString *const str, const size_t tabs)
{
	for( size_t i=0; i<tabs; i++ )
		harbol_string_add_cstr(str, "\t");
}

HARBOL_EXPORT struct HarbolString harbol_cfg_to_str(const struct HarbolMap *const map)
{
	static size_t tabs = 0;
	struct HarbolString cfg_str = harbol_string_make(NULL, &( bool ){false});
	struct HarbolString *str = &cfg_str;
	
	for( size_t i=0; i<map->len; i++ ) {
		const struct HarbolVariant *const var = ( const struct HarbolVariant* )(map->datum[i]);
		const union ConfigVal cv = { var->data };
		
		_concat_tabs(str, tabs);
		harbol_string_format(str, false, "\"%s\": ", ( const char* )(map->keys[i]));
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
				harbol_string_add_cstr(str, *cv.b ? "true\n" : "false\n");
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

static NO_NULL bool harbol_cfg_parse_target_path(const char key[static 1], struct HarbolString *const restrict str)
{
	/// parse something like: "root.section1.section2.section3.\\.dotsection"
	const char *iter = key;
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

static NO_NULL struct HarbolVariant *_get_var(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	/// first check if we're getting a singular value OR we iterate through a sectional path.
	const char *dot = strchr(key, '.');
	
	/// Patch: dot and escaped dot glitching out the hashmap hashing...
	if( dot==NULL || (dot > key && (dot[-1] == '\\')) ) {
		struct HarbolVariant *const restrict var = harbol_map_key_get(cfgmap, key, strlen(key)+1);
		return( var==NULL || var->tag==HarbolCfgType_Null ) ? NULL : var;
	}
	
	/// ok, not a singular value, iterate to the specific map section then.
	/// parse the target key first.
	const char *iter = key;
	struct HarbolString
		sectionstr = {0},
		targetstr = {0}
	;
	
	harbol_cfg_parse_target_path(key, &targetstr);
	const struct HarbolMap *itermap = cfgmap;
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
			itermap = *( const struct HarbolMap** )(var->data);
		}
	}
	harbol_string_clear(&sectionstr);
	harbol_string_clear(&targetstr);
	return var;
}

HARBOL_EXPORT struct HarbolMap *harbol_cfg_get_section(const struct HarbolMap *const restrict cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Map ) ? NULL : *( struct HarbolMap** )(var->data);
}

HARBOL_EXPORT char *harbol_cfg_get_cstr(const struct HarbolMap *const cfgmap, const char key[static 1], size_t *const len)
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	if( var==NULL || var->tag != HarbolCfgType_String ) {
		return NULL;
	} else {
		const struct HarbolString *const str = *( struct HarbolString** )(var->data);
		*len = str->len;
		return str->cstr;
	}
}

HARBOL_EXPORT struct HarbolString *harbol_cfg_get_str(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_String ) ? NULL : *( struct HarbolString** )(var->data);
}

HARBOL_EXPORT floatmax_t *harbol_cfg_get_float(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Float ) ? NULL : ( floatmax_t* )(var->data);
}

HARBOL_EXPORT intmax_t *harbol_cfg_get_int(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Int ) ? NULL : ( intmax_t* )(var->data);
}

HARBOL_EXPORT bool *harbol_cfg_get_bool(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Bool ) ? NULL : ( bool* )(var->data);
}

HARBOL_EXPORT union HarbolColor *harbol_cfg_get_color(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Color ) ? NULL : ( union HarbolColor* )(var->data);
}


HARBOL_EXPORT struct HarbolVec4D *harbol_cfg_get_vec4D(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL || var->tag != HarbolCfgType_Vec4D ) ? NULL : ( struct HarbolVec4D* )(var->data);
}

HARBOL_EXPORT enum HarbolCfgType harbol_cfg_get_type(const struct HarbolMap *const cfgmap, const char key[static 1])
{
	const struct HarbolVariant *const var = _get_var(cfgmap, key);
	return( var==NULL ) ? HarbolCfgType_Invalid : var->tag;
}

HARBOL_EXPORT bool harbol_cfg_set_str(struct HarbolMap *const restrict cfgmap, const char keypath[restrict static 1], const struct HarbolString str, const bool override_convert)
{
	return harbol_cfg_set_cstr(cfgmap, keypath, str.cstr, override_convert);
}

HARBOL_EXPORT bool harbol_cfg_set_cstr(struct HarbolMap *const restrict cfgmap, const char key[static 1], const char cstr[static 1], const bool override_convert)
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL )
		return false;
	else if( var->tag != HarbolCfgType_String ) {
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

HARBOL_EXPORT bool harbol_cfg_set_float(struct HarbolMap *const restrict cfgmap, const char key[static 1], const floatmax_t val, const bool override_convert)
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL )
		return false;
	else if( var->tag != HarbolCfgType_Float ) {
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

HARBOL_EXPORT bool harbol_cfg_set_int(struct HarbolMap *const restrict cfgmap, const char key[static 1], const intmax_t val, const bool override_convert)
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL )
		return false;
	else if( var->tag != HarbolCfgType_Int ) {
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

HARBOL_EXPORT bool harbol_cfg_set_bool(struct HarbolMap *const restrict cfgmap, const char key[static 1], const bool val, const bool override_convert)
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL )
		return false;
	else if( var->tag != HarbolCfgType_Bool ) {
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

HARBOL_EXPORT bool harbol_cfg_set_color(struct HarbolMap *const restrict cfgmap, const char key[static 1], const union HarbolColor val, const bool override_convert)
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL )
		return false;
	else if( var->tag != HarbolCfgType_Color ) {
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

HARBOL_EXPORT bool harbol_cfg_set_vec4D(struct HarbolMap *const restrict cfgmap, const char key[static 1], const struct HarbolVec4D val, const bool override_convert)
{
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

HARBOL_EXPORT bool harbol_cfg_set_to_null(struct HarbolMap *const restrict cfgmap, const char key[static 1])
{
	struct HarbolVariant *const restrict var = _get_var(cfgmap, key);
	if( var==NULL ) {
		return false;
	}
	
	_harbol_cfgkey_clear(var);
	*var = harbol_variant_make(&( char ){0}, 1, HarbolCfgType_Null, &( bool ){0});
	return true;
}

static NO_NULL void _write_tabs(FILE *const file, const size_t tabs)
{
	for( size_t i=0; i<tabs; i++ )
		fputs("\t", file);
}

static NO_NULL bool _harbol_cfg_build_file(const struct HarbolMap *const map, FILE *const file, const size_t tabs)
{
	for( size_t i=0; i<map->len; i++ ) {
		const struct HarbolVariant *const v = ( const struct HarbolVariant* )(map->datum[i]);
		const int32_t type = v->tag;
		_write_tabs(file, tabs);
		/// using double pointer iterators as we need the key.
		fprintf(file, "\"%s\": ", ( const char* )(map->keys[i]));
		
		const union ConfigVal cv = { v->data };
		switch( type ) {
			case HarbolCfgType_Null:
				fputs("null\n", file); break;
			case HarbolCfgType_Map:
				fputs("{\n", file);
				_harbol_cfg_build_file(*cv.section, file, tabs+1);
				_write_tabs(file, tabs);
				fputs("}\n", file);
				break;
			
			case HarbolCfgType_String:
				fprintf(file, "\"%s\"\n", (*cv.str)->cstr); break;
			case HarbolCfgType_Float:
				fprintf(file, "%" PRIfMAX "\n", *cv.f); break;
			case HarbolCfgType_Int:
				fprintf(file, "%" PRIiMAX "\n", *cv.i); break;
			case HarbolCfgType_Bool:
				fprintf(file, "%s\n", (*cv.b)? "true" : "false"); break;
			case HarbolCfgType_Color:
				fprintf(file, "c[ %u, %u, %u, %u ]\n", cv.c->bytes.r, cv.c->bytes.g, cv.c->bytes.b, cv.c->bytes.a); break;
			case HarbolCfgType_Vec4D:
				fprintf(file, "v[ %" PRIf32 ", %" PRIf32 ", %" PRIf32 ", %" PRIf32 " ]\n", cv.v->x, cv.v->y, cv.v->z, cv.v->w); break;
		}
	}
	return true;
}

HARBOL_EXPORT bool harbol_cfg_build_file(const struct HarbolMap *const cfg, const char filename[static 1], const bool overwrite)
{
	FILE *restrict cfgfile = fopen(filename, overwrite ? "w+" : "a+");
	if( cfgfile==NULL ) {
		fputs("harbol_cfg_build_file :: unable to create file.\n", stderr);
		return false;
	}
	const bool result = _harbol_cfg_build_file(cfg, cfgfile, 0);
	fclose(cfgfile); cfgfile=NULL;
	return result;
}
