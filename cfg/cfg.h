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


enum HarbolCfgType {
	HarbolCfgType_Invalid = -1,
	HarbolCfgType_Null,
	HarbolCfgType_Map,    /// ordered hashmap type
	HarbolCfgType_String, /// string type
	HarbolCfgType_Float,  /// floatmax_t, as defined by Harbol.
	HarbolCfgType_Int,    /// intmax_t aka long long int
	HarbolCfgType_Bool,   /// bool
	HarbolCfgType_Color,  /// 4 byte color array
	HarbolCfgType_Vec4D,  /// 16 byte float32 array
};

union HarbolColor {
	uint32_t int32;
	struct{ uint8_t r,g,b,a; } bytes;
};

struct HarbolVec4D {
	float32_t x,y,z,w;
};


HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_parse_file(const char filename[]);
HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_parse_cstr(const char cstr[]);
HARBOL_EXPORT NO_NULL void harbol_cfg_free(struct HarbolMap **cfgref);
HARBOL_EXPORT NO_NULL struct HarbolString harbol_cfg_to_str(const struct HarbolMap *cfg);

HARBOL_EXPORT NO_NULL struct HarbolMap *harbol_cfg_get_section(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL char *harbol_cfg_get_cstr(const struct HarbolMap *cfg, const char keypath[], size_t *len);
HARBOL_EXPORT NO_NULL struct HarbolString *harbol_cfg_get_str(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL floatmax_t *harbol_cfg_get_float(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL intmax_t *harbol_cfg_get_int(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL bool *harbol_cfg_get_bool(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL union HarbolColor *harbol_cfg_get_color(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL struct HarbolVec4D *harbol_cfg_get_vec4D(const struct HarbolMap *cfg, const char keypath[]);
HARBOL_EXPORT NO_NULL enum HarbolCfgType harbol_cfg_get_type(const struct HarbolMap *cfg, const char keypath[]);

HARBOL_EXPORT NO_NULL bool harbol_cfg_set_str(struct HarbolMap *cfg, const char keypath[], struct HarbolString str, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_cstr(struct HarbolMap *cfg, const char keypath[], const char cstr[], bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_float(struct HarbolMap *cfg, const char keypath[], floatmax_t fltval, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_int(struct HarbolMap *cfg, const char keypath[], intmax_t ival, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_bool(struct HarbolMap *cfg, const char keypath[], bool boolval, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_color(struct HarbolMap *cfg, const char keypath[], union HarbolColor color, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_vec4D(struct HarbolMap *cfg, const char keypath[], struct HarbolVec4D vec4d, bool override_convert);
HARBOL_EXPORT NO_NULL bool harbol_cfg_set_to_null(struct HarbolMap *cfg, const char keypath[]);

#ifdef C11
#	define harbol_cfg_set(cfg, keypath, val, override)  _Generic((val)+0, \
															struct HarbolString : harbol_cfg_set_str, \
															char* : harbol_cfg_set_cstr, \
															const char* : harbol_cfg_set_cstr, \
															float32_t : harbol_cfg_set_float, \
															float64_t : harbol_cfg_set_float, \
															floatmax_t : harbol_cfg_set_float, \
															int32_t : harbol_cfg_set_int, \
															uint32_t : harbol_cfg_set_int, \
															int64_t : harbol_cfg_set_int, \
															intmax_t : harbol_cfg_set_int, \
															bool : harbol_cfg_set_bool, \
															union HarbolColor : harbol_cfg_set_color, \
															struct HarbolVec4D : harbol_cfg_set_vec4D) \
														((cfg), (keypath), (val), (override))
#endif

HARBOL_EXPORT NO_NULL bool harbol_cfg_build_file(const struct HarbolMap *cfg, const char filename[], bool overwrite);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_CFG_INCLUDED */
