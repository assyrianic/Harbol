#ifndef HARBOL_CFG_INCLUDED
#	define HARBOL_CFG_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"
#include "../msg_sys/msg_sys.h"
#include "../map/map.h"
#include "../variant/variant.h"
#include "../lex/lex.h"
#include "../math/math_parser.h"


/** CFG Parser in EBNF grammar:
 * keyval  = string [':'] ( value | section ) [','] .
 * section = '{' *keyval '}' .
 * value   = string | number | vec | "true" | "false" | "null" | "iota" | "<FILE>" .
 * matrix  = '[' number [','] [number] [','] [number] [','] [number] ']' .
 * vec     = ('v' | 'c') matrix .
 * string  = '"' chars '"' | "'" chars "'" .
 */

enum HarbolCfgType {
	HarbolCfgType_Invalid = -1,
	HarbolCfgType_Null,
	HarbolCfgType_Map,    /// ordered hashmap type
	HarbolCfgType_String, /// string type
	HarbolCfgType_Float,  /// floatmax_t, as defined by Harbol
	HarbolCfgType_Int,    /// intmax_t aka long long int or bigger
	HarbolCfgType_Bool,   /// bool
	HarbolCfgType_Color,  /// 4 byte color array
	HarbolCfgType_Vec4D,  /// 16 byte float32 array
};

union HarbolColor {
	uint32_t uint32;
	struct{ uint8_t r,g,b,a; } bytes;
	uint8_t array[sizeof(uint32_t)];
};

struct HarbolVec4D {
	float32_t x,y,z,w;
};


HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_parse_file(char const filename[]);
HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_parse_cstr(char const cstr[]);
HARBOL_EXPORT NO_NULL void harbol_cfg_free(struct HarbolMap **cfgref);
HARBOL_EXPORT NO_NULL struct HarbolString harbol_cfg_to_str(struct HarbolMap const *cfg);

HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_get_section(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL char *harbol_cfg_get_cstr(struct HarbolMap const *cfg, char const keypath[], size_t *len);
HARBOL_EXPORT NO_NULL struct HarbolString *harbol_cfg_get_str(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL floatmax_t *harbol_cfg_get_float(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL intmax_t *harbol_cfg_get_int(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL bool *harbol_cfg_get_bool(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL union HarbolColor *harbol_cfg_get_color(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL struct HarbolVec4D *harbol_cfg_get_vec4D(struct HarbolMap const *cfg, char const keypath[]);
HARBOL_EXPORT NO_NULL enum HarbolCfgType harbol_cfg_get_type(struct HarbolMap const *cfg, char const keypath[]);

HARBOL_EXPORT NO_NULL bool harbol_cfg_set_str(struct HarbolMap *cfg, char const keypath[], struct HarbolString str, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_cstr(struct HarbolMap *cfg, char const keypath[], char const cstr[], bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_float(struct HarbolMap *cfg, char const keypath[], floatmax_t fltval, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_int(struct HarbolMap *cfg, char const keypath[], intmax_t ival, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_bool(struct HarbolMap *cfg, char const keypath[], bool boolval, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_color(struct HarbolMap *cfg, char const keypath[], union HarbolColor color, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_vec4D(struct HarbolMap *cfg, char const keypath[], struct HarbolVec4D vec4d, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_to_null(struct HarbolMap *cfg, char const keypath[]);

/// requires expressions be of string type.
HARBOL_EXPORT NEVER_NULL(1, 2) floatmax_t harbol_cfg_calc_math(struct HarbolMap const *cfg, char const keypath[], HarbolMathVarFunc *var_func, void *data, size_t data_len);

#ifdef C11
#	define harbol_cfg_set(cfg, keypath, val, override)  _Generic((val)+0, \
															struct HarbolString : harbol_cfg_set_str,   \
															char*               : harbol_cfg_set_cstr,  \
															char const*         : harbol_cfg_set_cstr,  \
															float32_t           : harbol_cfg_set_float, \
															float64_t           : harbol_cfg_set_float, \
															floatmax_t          : harbol_cfg_set_float, \
															int32_t             : harbol_cfg_set_int,   \
															uint32_t            : harbol_cfg_set_int,   \
															int64_t             : harbol_cfg_set_int,   \
															intmax_t            : harbol_cfg_set_int,   \
															bool                : harbol_cfg_set_bool,  \
															union HarbolColor   : harbol_cfg_set_color, \
															struct HarbolVec4D  : harbol_cfg_set_vec4D) \
														( (cfg), (keypath), (val), (override) )
#endif

HARBOL_EXPORT NO_NULL bool harbol_cfg_build_file(struct HarbolMap const *cfg, char const filename[], bool overwrite);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_CFG_INCLUDED */
