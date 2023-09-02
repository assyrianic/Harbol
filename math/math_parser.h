#ifndef HARBOL_MATH_INCLUDED
#	define HARBOL_MATH_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"
#include "../lex/lex.h"


typedef floatmax_t HarbolMathFunc(floatmax_t value);
typedef void HarbolMathVarFunc(
	char const       var_name[],
	size_t           var_len,
	floatmax_t      *value,
	HarbolMathFunc **math_func,
	void            *data,
	size_t           data_len,
	bool            *is_func
);

/**
```ebnf
Expr      = AddExpr .
AddExpr   = MulExpr *( ('+' | '-') MulExpr ) .
MulExpr   = PowExpr *( ('*' | '/') PowExpr ) .
PowExpr   = UnaryExpr *( '^' UnaryExpr ) .
UnaryExpr = *( '-' | '+' ) Factor .
Factor    = number | ident | func ( '(' | '[' )? UnaryExpr , PowExpr | '(' Expr ')' | '[' Expr ']' .
```
 */


HARBOL_EXPORT NEVER_NULL(1) floatmax_t harbol_math_parse_expr(char const expression[], HarbolMathVarFunc *var_func, void *data, size_t data_len);


static inline floatmax_t math_parse_sin(floatmax_t const x)      { return sin(x); }
static inline floatmax_t math_parse_cos(floatmax_t const x)      { return cos(x); }
static inline floatmax_t math_parse_tan(floatmax_t const x)      { return tan(x); }
static inline floatmax_t math_parse_arcsin(floatmax_t const x)   { return asin(x); }
static inline floatmax_t math_parse_arccos(floatmax_t const x)   { return acos(x); }
static inline floatmax_t math_parse_arctan(floatmax_t const x)   { return atan(x); }
static inline floatmax_t math_parse_ln(floatmax_t const x)       { return log(x); }
static inline floatmax_t math_parse_log(floatmax_t const x)      { return log10(x); }
static inline floatmax_t math_parse_floor(floatmax_t const x)    { return floor(x); }
static inline floatmax_t math_parse_ceil(floatmax_t const x)     { return ceil(x); }
static inline floatmax_t math_parse_round(floatmax_t const x)    { return round(x); }
static inline floatmax_t math_parse_fraction(floatmax_t const x) { return x - floor(x); }
static inline floatmax_t math_parse_radians(floatmax_t const x)  { return x * (math_parse_arccos(-1.0) / 180.0); }
static inline floatmax_t math_parse_degrees(floatmax_t const x)  { return x * (180.0 / math_parse_arccos(-1.0)); }

static inline HarbolMathFunc *harbol_math_check_default_funcs(char const var_name[]) {
	char const *const default_func_names[] = {
		"sin", "cos", "tan",   "arcsin", "arccos", "arctan",
		"ln",  "log", "floor", "ceil",   "round",  "fraction",
		"radians",    "degrees"
	};
	HarbolMathFunc *const default_funcs[] = {
		math_parse_sin,    math_parse_cos,      math_parse_tan,
		math_parse_arcsin, math_parse_arccos,   math_parse_arctan,
		math_parse_ln,     math_parse_log,      math_parse_floor,   math_parse_ceil,
		math_parse_round,  math_parse_fraction, math_parse_radians, math_parse_degrees
	};
	for( size_t i=0; i < (sizeof default_func_names / sizeof default_func_names[0]); i++ ) {
		if( !strcmp(var_name, default_func_names[i]) ) {
			return default_funcs[i];
		}
	}
	return NULL;
}

static inline bool harbol_math_check_default_vars(char const var_name[], floatmax_t *const restrict value) {
	char const *const default_var_names[] = {
		"e", "pi"
	};
	floatmax_t const default_var_values[] = {
		exp(( floatmax_t )(1.0)),   /// e
		acos(( floatmax_t )(-1.0)), /// π
	};
	
	for( size_t i=0; i < (sizeof default_var_names / sizeof default_var_names[0]); i++ ) {
		if( !strcmp(var_name, default_var_names[i]) ) {
			*value = default_var_values[i];
			return true;
		}
	}
	return false;
}

static inline void harbol_math_default_var_func(
	char   const                    var_name[],
	size_t const                    var_len,
	floatmax_t      *const restrict value,
	HarbolMathFunc **const restrict math_func,
	void            *const          data,
	size_t const                    data_len,
	bool            *const restrict is_func
) {
	(void)(var_len);
	(void)(data);
	(void)(data_len);
	if( harbol_math_check_default_vars(var_name, value) ) {
		return;
	}
	
	HarbolMathFunc *fn = harbol_math_check_default_funcs(var_name);
	if( fn != NULL ) {
		*math_func = fn;
		*is_func   = true;
	}
}


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MATH_INCLUDED */
