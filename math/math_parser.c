#include "math_parser.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


enum {
	LEXEME_SIZE = 64,
};

enum HarbolMathTokenType {
	TokenInvalid,
	TokenVar, TokenNum,
	
	/// delimiters.
	TokenLParen, TokenRParen,
	TokenLBrack, TokenRBrack,
	
	/// math operators.
	TokenPlus, TokenSub,
	TokenMul, TokenDiv,
	TokenPow,
};

struct HarbolMathToken {
	floatmax_t               val;
	size_t                   size;
	enum HarbolMathTokenType tag;
	char                     lexeme[LEXEME_SIZE];
};

struct HarbolMathLexer {
	struct HarbolMathToken   tok;
	HarbolMathVarFunc       *var_func;
	void                    *data;
	size_t                   expr_len, idx, data_len;
};


static NO_NULL bool _harbol_math_lex_decimal(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	bool has_dot = false;
	while( expression[ls->idx] != 0 && (isalnum(expression[ls->idx]) || expression[ls->idx]=='.') ) {
		switch( expression[ls->idx] ) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->tok.size++;
				ls->idx++;
				break;
			}
			case '.': {
				if( has_dot ) {
					//LogError("ConfigMap-Math :: extra dot in decimal literal");
					return false;
				}
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->tok.size++;
				ls->idx++;
				has_dot = true;
				break;
			}
			default: {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->tok.size++;
				//LogError("ConfigMap-Math :: invalid decimal literal: '%s'", ls->tok.lexeme);
				return false;
			}
		}
	}
	return true;
}

static NO_NULL bool _harbol_math_get_token(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	if( ls->expr_len==0 ) {
		ls->expr_len = strlen(expression);
	}
	memset(&ls->tok, 0, sizeof ls->tok);
	while( ls->idx < ls->expr_len ) {
		switch( expression[ls->idx] ) {
			case ' ': case '\t': case '\n': case '\r': {
				ls->idx++;
				break;
			}
			case '.': 
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': {
				ls->tok.tag = TokenNum;
				/// Decimal/Float.
				bool const res = _harbol_math_lex_decimal(ls, expression);
				if( res ) {
					ls->tok.val = strtofmax(ls->tok.lexeme, NULL);
				}
				return res;
			}
			case '(': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenLParen;
				return true;
			}
			case ')': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenRParen;
				return true;
			}
			case '[': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenLBrack;
				return true;
			}
			case ']': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenRBrack;
				return true;
			}
			case '+': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenPlus;
				return true;
			}
			case '-': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenSub;
				return true;
			}
			case '*': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenMul;
				return true;
			}
			case '/': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenDiv;
				return true;
			}
			case '^': {
				ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
				ls->idx++;
				ls->tok.size++;
				ls->tok.tag = TokenPow;
				return true;
			}
			default: {
				bool got_something = false;
				while( expression[ls->idx] != 0 && isalnum(expression[ls->idx]) && ls->tok.size < LEXEME_SIZE ) {
					ls->tok.lexeme[ls->tok.size] = expression[ls->idx];
					ls->idx++;
					ls->tok.size++;
					got_something = true;
				}
				if( got_something ) {
					ls->tok.tag = TokenVar;
					return true;
				}			
				//LogError("ConfigMap-Math :: invalid expression token '%s'.", ls->tok.lexeme);
				return false;
			}
		}
	}
	return false;
}


floatmax_t harbol_math_parse_add_expr  (struct HarbolMathLexer *ls, char const expression[]);
floatmax_t harbol_math_parse_mul_expr  (struct HarbolMathLexer *ls, char const expression[]);
floatmax_t harbol_math_parse_pow_expr  (struct HarbolMathLexer *ls, char const expression[]);
floatmax_t harbol_math_parse_unary_expr(struct HarbolMathLexer *ls, char const expression[]);
floatmax_t harbol_math_parse_factor    (struct HarbolMathLexer *ls, char const expression[]);

/// Expr = AddExpr .
HARBOL_EXPORT floatmax_t harbol_math_parse_expr(char const expression[const restrict static 1], HarbolMathVarFunc *var_func, void *const restrict data, size_t const data_len) {
	struct HarbolMathLexer ls = {
		.var_func = var_func,
		.data     = data,
		.data_len = data_len,
	};
	_harbol_math_get_token(&ls, expression);
	return harbol_math_parse_add_expr(&ls, expression);
}

/// AddExpr = MulExpr *( ('+' | '-') MulExpr ) .
floatmax_t harbol_math_parse_add_expr(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	floatmax_t val = harbol_math_parse_mul_expr(ls, expression);
	if( ls->tok.tag==TokenPlus || ls->tok.tag==TokenSub ) {
		while( ls->tok.tag==TokenPlus || ls->tok.tag==TokenSub ) {
			enum HarbolMathTokenType const t = ls->tok.tag;
			_harbol_math_get_token(ls, expression);
			if( t==TokenPlus ) {
				val += harbol_math_parse_mul_expr(ls, expression);
			} else if( t==TokenSub ) {
				val -= harbol_math_parse_mul_expr(ls, expression);
			}
		}
	}
	return val;
}

/// MulExpr = PowExpr *( ('*' | '/') PowExpr ) .
floatmax_t harbol_math_parse_mul_expr(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	floatmax_t val = harbol_math_parse_pow_expr(ls, expression);
	if( ls->tok.tag==TokenMul || ls->tok.tag==TokenDiv ) {
		while( ls->tok.tag==TokenMul || ls->tok.tag==TokenDiv ) {
			enum HarbolMathTokenType const t = ls->tok.tag;
			_harbol_math_get_token(ls, expression);
			if( t==TokenMul ) {
				val *= harbol_math_parse_pow_expr(ls, expression);
			} else if( t==TokenDiv ) {
				val /= harbol_math_parse_pow_expr(ls, expression);
			}
		}
	}
	return val;
}

/// PowExpr = UnaryExpr *( '^' UnaryExpr ) .
floatmax_t harbol_math_parse_pow_expr(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	floatmax_t val = harbol_math_parse_unary_expr(ls, expression);
	if( ls->tok.tag==TokenPow ) {
		while( ls->tok.tag==TokenPow ) {
			_harbol_math_get_token(ls, expression);
			val = pow(val, harbol_math_parse_unary_expr(ls, expression));
		}
	}
	return val;
}

/// UnaryExpr = *( '-' | '+' ) Factor .
floatmax_t harbol_math_parse_unary_expr(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	if( ls->tok.tag==TokenSub ) {
		_harbol_math_get_token(ls, expression);
		return -harbol_math_parse_unary_expr(ls, expression);
	} else if( ls->tok.tag==TokenPlus ) {
		_harbol_math_get_token(ls, expression);
		return fabs( harbol_math_parse_unary_expr(ls, expression) );
	}
	return harbol_math_parse_factor(ls, expression);
}


/// Factor  = number | ident | func UnaryExpr | '(' Expr ')' | '[' Expr ']' .
floatmax_t harbol_math_parse_factor(struct HarbolMathLexer *const restrict ls, char const expression[const restrict static 1]) {
	union {
		uintmax_t  u;
		floatmax_t f;
	} value = { UINTMAX_MAX };
	switch( ls->tok.tag ) {
		case TokenNum: {
			floatmax_t const f = ls->tok.val;
			_harbol_math_get_token(ls, expression);
			return f;
		}
		case TokenVar: {
			char lexeme[LEXEME_SIZE] = {0};
			memcpy(lexeme, ls->tok.lexeme, sizeof ls->tok.lexeme);
			size_t const len = ls->tok.size;
			_harbol_math_get_token(ls, expression);
			
			/// void HarbolMathVarFunc(char const var_name[], size_t var_len, floatmax_t *value, HarbolMathFunc **math_func, void *data, size_t data_len, bool *is_func);
			if( ls->var_func != NULL ) {
				HarbolMathFunc *math_fn = NULL;
				bool is_func = false;
				(*ls->var_func)(lexeme, len, &value.f, &math_fn, ls->data, ls->data_len, &is_func);
				if( is_func && math_fn != NULL ) {
					return (*math_fn)( harbol_math_parse_unary_expr(ls, expression) );
				}
			}
			return value.f;
		}
		case TokenLParen: {
			_harbol_math_get_token(ls, expression);
			floatmax_t const f = harbol_math_parse_add_expr(ls, expression);
			if( ls->tok.tag != TokenRParen ) {
				//LogError("ConfigMap-Math :: expected ')' bracket but got '%s'", ls->tok.lexeme);
				return value.f;
			}
			_harbol_math_get_token(ls, expression);
			return f;
		}
		case TokenLBrack: {
			_harbol_math_get_token(ls, expression);
			floatmax_t const f = harbol_math_parse_add_expr(ls, expression);
			if( ls->tok.tag != TokenRBrack ) {
				//LogError("ConfigMap-Math :: expected ']' bracket but got '%s'", ls->tok.lexeme);
				return value.f;
			}
			_harbol_math_get_token(ls, expression);
			return f;
		}
		default: {
			return value.f;
		}
	}
}
