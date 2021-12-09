#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "lex.h"

void test_harbol_lex(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_lex_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_lex(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_lex(FILE *const debug_stream)
{
	fputs("\nlex tools :: test C hex.\n", debug_stream);
	const char *c_hexs[] = {
		"0X55", /// correct
		"0X5fl", /// correct
		"0x5'5LLU", /// correct
		"0X5full", /// correct
		"0x0.3p10", /// correct
		"0x1.2p3", /// correct
		"0x1p+1", /// correct
		"0x1.b7p-1", /// correct
		"0x3.3333333333334p-5", /// correct
		"0x", /// bad
		"0x.f", /// bad
		"0x1.f", /// bad
	};
	for( const char **i=&c_hexs[0]; i<1[&c_hexs]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_c_style_hex(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test Go hex.\n", debug_stream);
	const char *go_hex[] = {
		"0xBadFace", /// correct
		"0xBad_Face", /// correct
		"0x_67_7a_2f_cc_40_c6", /// correct
		"0x1p-2",       /// == 0.25
		"0x2.p10",      /// == 2048.0
		"0x1.Fp+0",     /// == 1.9375
		"0X.8p-0",      /// == 0.5
		"0X_1FFFP-16",  /// == 0.1249847412109375
		"0x15e-2",      /// == 0x15e - 2 (integer subtraction)
		"0x3.3333333333334p-5", /// correct
		"0x.p1", /// bad
		"0x1.5e-2", /// bad
		"0x_67_7a_2f_cc_40_c6_", /// bad
		"0_xBadFace", /// bad
	};
	for( const char **i=&go_hex[0]; i<1[&go_hex]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_go_style_hex(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test C decimal.\n", debug_stream);
	const char *c_dec[] = {
		"344ULL", /// correct
		"33", /// correct
		"33_45_34", /// correct
		".0", /// correct
		".344", /// correct
		".34e4", /// correct
		".34e4f", /// correct
		"0.f", /// correct
		"0.34e4f", /// correct
		"3.-3", /// good
		"3.", /// good
		"3.e-3", /// good
		"3.e--3", /// good
		"0f", /// bad
		".34ef", /// bad
		"0.34ef", /// bad
		"0.34e1ULL", /// bad
		"34e1ULL", /// bad
	};
	for( const char **i=&c_dec[0]; i<1[&c_dec]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_c_style_decimal(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test Go decimal.\n", debug_stream);
	const char *go_dec[] = {
		"0.",
		"72.40",
		"072.40",
		"2.71828",
		"1.e+0",
		"6.67428e-11",
		"1E6",
		".25",
		".12345E+5",
		"1_5.",
		"0.15e+0_2",
		"1_.5", /// bad
		"1._5", /// bad
		"1.5_e1", /// bad
		"1.5e_1", /// bad
		"1.5e1_" /// bad
	};
	for( const char **i=&go_dec[0]; i<1[&go_dec]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_go_style_decimal(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	
	fputs("\nlex tools :: test Go String.\n", debug_stream);
	const char *go_strs[] = {
		"`raw\\nstring`",
		"\"foo\\x61 bar\"",
		"\"foo\\141bar\"",
		"\"foo\\u0710 bar\"",
		"\"日本語\"",
		"\"\\u65e5本\\U00008a9e\""
	};
	for( const char **i=&go_strs[0]; i<1[&go_strs]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const int res = lex_go_style_str(*i, &end, &lexeme);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test C octal.\n", debug_stream);
	const char *c_oct[] = {
		"0553", /// correct
		"0553ULL", /// correct
		"0553u", /// correct
		"0553llu", /// correct
		"0777", /// correct
		"0553LUL", /// bad
		"553", /// bad
		"078", /// bad
	};
	for( const char **i=&c_oct[0]; i<1[&c_oct]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_c_style_octal(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test Go octal.\n", debug_stream);
	const char *go_oct[] = {
		"0o553", /// correct
		"0o777", /// correct
		"0o78", /// bad
		"0o553ULL", /// bad
		"0o553LUL", /// bad
		"0o553ull", /// bad
		"553", /// bad
		"0553", /// bad
	};
	for( const char **i=&go_oct[0]; i<1[&go_oct]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const int res = lex_go_style_octal(*i, &end, &lexeme);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	
	fputs("\nlex tools :: test C binary.\n", debug_stream);
	const char *c_binary[] = {
		"0b11101", /// correct
		"0b101ULL", /// correct
		"0b1u", /// correct
		"0b101llu", /// correct
		"b1101010", /// bad
		"0b10002", /// bad
		"011101010", /// bad
	};
	for( const char **i=&c_binary[0]; i<1[&c_binary]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const int res = lex_c_style_binary(*i, &end, &lexeme);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test Go binary.\n", debug_stream);
	const char *go_binary[] = {
		"0b11101", /// correct
		"0b110_101", /// correct
		"b1101010", /// bad
		"0b10002", /// bad
		"011101010", /// bad
	};
	for( const char **i=&go_binary[0]; i<1[&go_binary]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const int res = lex_go_style_binary(*i, &end, &lexeme);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test C various numbers.\n", debug_stream);
	const char *c_nums[] = {
		"0X5'5", /// correct
		"0X5fl", /// correct
		"0x55LLU", /// correct
		"0X5full", /// correct
		"0X'CAFE'BABE", /// correct
		"0x0.3p10", /// correct
		"0x1.2p3", /// correct
		"0x1p+1", /// correct
		"0x1.b7p-1", /// correct
		"0x0.3_p10", /// correct
		"0x3.3333333333334p-5", /// correct
		"0x1.f", /// bad
		"0553", /// correct
		"05'53ULL", /// correct
		"0553u", /// correct
		"0553llu", /// correct
		"07'77", /// correct
		"0553LUL", /// bad
		"55'3", /// bad
		"078", /// bad
		".llu", /// bad
	};
	for( const char **i=&c_nums[0]; i<1[&c_nums]; i++ ) {
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		bool is_float = false;
		const int res = lex_c_style_number(*i, &end, &lexeme, &is_float);
		fprintf(debug_stream, "result: %s :: lexeme: '%s' | is float? %s | err: %s\n", res==0 ? "yes" : "no", lexeme.cstr, is_float ? "yes" : "no", lex_get_err(res));
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test single-line comment lexing.\n", debug_stream);
	{
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const bool res = lex_single_line_comment("/// kektus \\      \n foobar  \\ \n bazbard", &end, &lexeme);
		fprintf(debug_stream, "result: %s :: comment: '%s'\n", res==0 ? "yes" : "no", lexeme.cstr);
		harbol_string_clear(&lexeme);
	}
	
	fputs("\nlex tools :: test multi-line comment lexing.\n", debug_stream);
	{
		struct HarbolString lexeme = harbol_string_make(NULL, &( bool ){false});
		const char *end = NULL;
		const bool res = lex_multi_line_comment("/** kektus \n foobar  \n bazbard */", &end, "*/", sizeof "*/"-1, &lexeme);
		fprintf(debug_stream, "result: %s :: comment: '%s'\n", res==0 ? "yes" : "no", lexeme.cstr);
		harbol_string_clear(&lexeme);
	}
	fputs("\nlex tools :: test converting utf8 to runes.\n", debug_stream);
	{
		size_t len = 0;
		struct HarbolString utf8 = { "ܩܙܛas日本語dsads", sizeof "ܩܙܛas日本語dsads" - 1 };
		int32_t *runes = utf8_to_rune(&utf8, &len);
		fprintf(debug_stream, "lex tools :: utf8 -> '%s' | '%zu'\nlex tools :: iterating runes (count: %zu).\n", utf8.cstr, utf8.len, len);
		for( size_t i=0; i<=len; i++ ) {
			fprintf(debug_stream, "runes[%zu]:: 'U+%.8X'\n", i, runes[i]);
		}
		fputs("\nlex tools :: test converting runes to utf8.\n", debug_stream);
		/// 'utf8'-s buffer is allocated here!
		utf8 = rune_to_utf8_str(runes, len);
		free(runes); runes = NULL;
		fprintf(debug_stream, "lex tools :: utf8 -> '%s' | '%zu'\n", utf8.cstr, utf8.len);
		harbol_string_clear(&utf8);
	}
}
