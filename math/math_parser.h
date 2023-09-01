#ifndef HARBOL_MATH_INCLUDED
#	define HARBOL_MATH_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"
#include "../lex/lex.h"


typedef floatmax_t HarbolMathFunc(floatmax_t value);
typedef void HarbolMathVarFunc(char const var_name[], size_t var_len, floatmax_t *value, HarbolMathFunc **math_func, void *data, size_t data_len, bool *is_func);

/**
```ebnf
Expr      = AddExpr .
AddExpr   = MulExpr *( ('+' | '-') MulExpr ) .
MulExpr   = PowExpr *( ('*' | '/') PowExpr ) .
PowExpr   = UnaryExpr *( '^' UnaryExpr ) .
UnaryExpr = *( '-' | '+' ) Factor .
Factor    = number | ident | func UnaryExpr | '(' Expr ')' | '[' Expr ']' .
```
 */


HARBOL_EXPORT NEVER_NULL(1) floatmax_t harbol_math_parse_expr(char const expression[], HarbolMathVarFunc *var_func, void *data, size_t data_len);


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MATH_INCLUDED */
