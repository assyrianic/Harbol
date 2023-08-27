#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "math_parser.h"

void test_harbol_math(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void) {
	FILE *debug_stream = fopen("harbol_math_output.txt", "w");
	if( debug_stream==NULL ) {
		return -1;
	}
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_math(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}

static inline floatmax_t my_log10(floatmax_t x) {
	return log10(x);
}
static inline floatmax_t my_log(floatmax_t x) {
	return log(x);
}

static void test_math_func(
	char const                      var_name[const restrict static 1],
	size_t const                    var_len,
	floatmax_t      *const restrict value,
	HarbolMathFunc **const restrict math_func,
	void            *const restrict data,
	size_t const                    data_len,
	bool            *const restrict is_func
) {
	(void)(var_len);
	(void)(data);
	(void)(data_len);
	if( !strcmp(var_name, "e") ) {
		*value = exp((floatmax_t)(1.0));
	} else if( !strcmp(var_name, "pi") ) {
		*value = acos((floatmax_t)(-1.0));
	} else if( !strcmp(var_name, "log10") ) {
		*math_func = my_log10;
		*is_func = true;
	} else if( !strcmp(var_name, "log") ) {
		*math_func = my_log;
		*is_func = true;
	}
}

void test_harbol_math(FILE *const debug_stream) {
	fputs("math parser :: test various expressions.\n", debug_stream);
	floatmax_t result = harbol_math_parse_expr("1 + 1", test_math_func, NULL, 0);
	fprintf(debug_stream, "'1 + 1' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("1 + e", test_math_func, NULL, 0);
	fprintf(debug_stream, "'1 + e' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("2^7", test_math_func, NULL, 0);
	fprintf(debug_stream, "'2^7' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("log e", test_math_func, NULL, 0);
	fprintf(debug_stream, "'log e' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("log10 10", test_math_func, NULL, 0);
	fprintf(debug_stream, "'log10 10' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("pi + 0", test_math_func, NULL, 0);
	fprintf(debug_stream, "'pi + 0' == '%" PRIfMAX "'\n", result);
	
	result = harbol_math_parse_expr("[(760.8 + 31) * (31 - 1)]^1.0341 + 2046", test_math_func, NULL, 0);
	fprintf(debug_stream, "'[(760.8 + 31) * (31 - 1)]^1.0341 + 2046' == '%" PRIfMAX "'\n", result);
	
	bool res;
	struct HarbolString new_expr = harbol_string_make("", &res);
	harbol_string_format(&new_expr, true, "%" PRIfMAX "%s", result, "^0.74074 / [log(31 + 2) / log 25]");
	result = harbol_math_parse_expr(new_expr.cstr, test_math_func, NULL, 0);
	fprintf(debug_stream, "'%s' == '%" PRIfMAX "'\n", new_expr.cstr, result);
	harbol_string_clear(&new_expr);
}
