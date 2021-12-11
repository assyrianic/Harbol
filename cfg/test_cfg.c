#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "cfg.h"

void test_harbol_cfg(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void)
{
	FILE *debug_stream = fopen("harbol_cfg_output.txt", "w");
	if( debug_stream==NULL )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_create(1000000);
	g_pool = &m;
#endif
	test_harbol_cfg(debug_stream);
	
	fclose(debug_stream); debug_stream=NULL;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool);
#endif
}


void test_harbol_cfg(FILE *const debug_stream)
{
	/// Test allocation and initializations
	fputs("cfg :: test allocation/initialization.\n", debug_stream);
	struct HarbolMap *cfg = harbol_cfg_parse_cstr("'section': { 'lel': null }");
	fprintf(debug_stream, "cfg ptr valid?: '%s'\n", cfg != NULL ? "yes" : "no");
	
	if( cfg != NULL ) {
		fputs("\ncfg :: testing config to string conversion.\n", debug_stream);
		struct HarbolString stringcfg = harbol_cfg_to_str(cfg);
		fprintf(debug_stream, "\ncfg :: \n%s\n", stringcfg.cstr);
		harbol_string_clear(&stringcfg);
	}
	
	/// Test realistic config file string
	fputs("cfg :: test realistic config file string.\n", debug_stream);
	const char *test_cfg = "'root': { \
		'firstName': 'John', \
		'lastName': 'Smith', \
		'isAlive': true, \
		'age': 0x18 , \
		'money': 35.42e4 \
		'myself': <FILE> \
		'address': { \
			'streetAddress': '21 2nd Street', \
			'city': 'New York', \
			'state': 'NY', \
			'postalCode': '10021-3100' \
		}, \
		'phoneNumbers.': { \
			'1' { \
				'type': 'home \\x5c', \
				'number': '212 555-1234' \
			}, \
			'2' { \
				'type': 'office', \
				'number': '646 555-4567' \
			}, \
			'3' { \
				'type': 'mobile', \
				'number': '123 456-7890' \
			} \
		}, \
		'colors': c[ 0xff, 0xff, 0xff, 0xaa ], \
		'origin': v[10.0f, 24.43, 25.0, 0xB.p+2], \
		'children': {}, \
		'spouse': null \
		'test_iota': { \
		    '<enum>': iota \
		    '<enum>': { \
		        'a': iota \
		        'b': iota \
		        'c': iota \
		        'd': iota \
		        'e': iota \
		    } \
		    '3': iota \
		    '4': iota \
		    '5': iota \
		    '6': iota \
		    '7': iota \
		} \
		'test_IOTA': { \
		    '1': IOTA \
		    '2': IOTA \
		    '3': IOTA \
		    '4': IOTA \
		    '5': IOTA \
		    '6': IOTA \
		    '7': IOTA \
		} \
	}";
	const clock_t start = clock();
	struct HarbolMap *larger_cfg = harbol_cfg_parse_cstr(test_cfg);
	const clock_t end = clock();
	printf("cfg parsing time: %f\n", (end-start)/(double)CLOCKS_PER_SEC);
	
	fprintf(debug_stream, "larger_cfg ptr valid?: '%s'\n", larger_cfg != NULL ? "yes" : "no");
	if( larger_cfg != NULL ) {
		fputs("\ncfg :: iterating realistic config.\n", debug_stream);
		struct HarbolString stringcfg = harbol_cfg_to_str(larger_cfg);
		fprintf(debug_stream, "\ncfg :: test config to string conversion:\n%s\n", stringcfg.cstr);
		harbol_string_clear(&stringcfg);
		
		fputs("\ncfg :: test retrieving sub section of realistic config.\n", debug_stream);
		struct HarbolMap *phone_numbers1 = harbol_cfg_get_section(larger_cfg, "root.phoneNumbers\\..1");
		printf("larger_cfg (%p) :: phone_numbers1 (%p) -> root.phoneNumbers\\..1\n", larger_cfg, phone_numbers1);
		if( phone_numbers1 ) {
			stringcfg = harbol_cfg_to_str(phone_numbers1);
			fprintf(debug_stream, "\nphone_numbers to string conversion: \n%s\n", stringcfg.cstr);
			harbol_string_clear(&stringcfg);
			
			fputs("\ncfg :: iterating phone_numbers1 subsection.\n", debug_stream);
			stringcfg = harbol_cfg_to_str(phone_numbers1);
			fprintf(debug_stream, "\nphone_numbers1 to string conversion: \n%s\n", stringcfg.cstr);
			harbol_string_clear(&stringcfg);
		}
		
		fputs("\ncfg :: test retrieving string value from subsection of realistic config.\n", debug_stream);
		size_t type_len = 0;
		char *type = harbol_cfg_get_cstr(larger_cfg, "root.phoneNumbers\\..1.type", &type_len);
		fprintf(debug_stream, "root.phoneNumbers\\..1.type type valid?: '%s'\n", type ? "yes" : "no");
		if( type ) {
			fprintf(debug_stream, "root.phoneNumbers\\..1.type: %s\n", type);
		}
		type = harbol_cfg_get_cstr(larger_cfg, "root.age", &type_len);
		fprintf(debug_stream, "root.age string type valid?: '%s'\n", type ? "yes" : "no");
		if( type ) {
			fprintf(debug_stream, "root.age: %s\n", type);
		}
		intmax_t *age = harbol_cfg_get_int(larger_cfg, "root.age");
		if( age )
			fprintf(debug_stream, "root.age int?: '%" PRIiMAX "'\n", *age);
		floatmax_t *money = harbol_cfg_get_float(larger_cfg, "root.money");
		if( money )
			fprintf(debug_stream, "root.money float?: '%" PRIfMAX "'\n", *money);
		
		union HarbolColor *color = harbol_cfg_get_color(larger_cfg, "root.colors");
		if( color )
			fprintf(debug_stream, "root.colors: '[%u, %u, %u, %u]'\n", color->bytes.r, color->bytes.g, color->bytes.b, color->bytes.a);
		
		fputs("\ncfg :: test override setting an existing key-value from null to a string type.\n", debug_stream);
		harbol_cfg_set_cstr(larger_cfg, "root.spouse", "Jane Smith", true);
		{
			struct HarbolString stringcfg = harbol_cfg_to_str(larger_cfg);
			fprintf(debug_stream, "\nadded spouse!: \n%s\n", stringcfg.cstr);
			harbol_string_clear(&stringcfg);
		}
		
		fputs("\ncfg :: test building cfg file!\n", debug_stream);
		fprintf(debug_stream, "\nconfig construction result: '%s'\n", harbol_cfg_build_file(larger_cfg, "large_cfg.ini", true) ? "success" : "failure");
		
		fputs("\ncfg :: test setting a key back to null\n", debug_stream);
		harbol_cfg_set_to_null(larger_cfg, "root.spouse");
		{
			struct HarbolString stringcfg = harbol_cfg_to_str(larger_cfg);
			fprintf(debug_stream, "\nremoved spouse!: \n%s\n", stringcfg.cstr);
			harbol_string_clear(&stringcfg);
		}
		
		fputs("\ncfg :: test getting the type of a key\n", debug_stream);
		fprintf(debug_stream, "Type of root.phoneNumbers\\.: %i\n", harbol_cfg_get_type(larger_cfg, "root.phoneNumbers\\."));
		fprintf(debug_stream, "Type of root.spouse: %i\n", harbol_cfg_get_type(larger_cfg, "root.spouse"));
		fprintf(debug_stream, "Type of root.money: %i\n", harbol_cfg_get_type(larger_cfg, "root.money"));
		fprintf(debug_stream, "Type of root.origin: %i\n", harbol_cfg_get_type(larger_cfg, "root.origin"));
		
		fputs("\ncfg :: test adding other cfg as a new section\n", debug_stream);
		{
			struct HarbolVariant var = harbol_variant_make(&cfg, sizeof cfg, HarbolCfgType_Map, &( bool ){0});
			harbol_map_insert(larger_cfg, "former lovers", sizeof "former lovers", &var, sizeof var);
			struct HarbolString stringcfg = harbol_cfg_to_str(larger_cfg);
			fprintf(debug_stream, "\nremoved spouse!: \n%s\n", stringcfg.cstr);
			harbol_string_clear(&stringcfg);
		}
		fputs("\ncfg :: test building newer cfg file!\n", debug_stream);
		fprintf(debug_stream, "\nconfig construction result: '%s'\n", harbol_cfg_build_file(larger_cfg, "large_cfg_new_sect.ini", true) ? "success" : "failure");
		harbol_cfg_free(&larger_cfg);
		fprintf(debug_stream, "cfg ptr valid?: '%s'\n", cfg ? "yes" : "no");
	}
	cfg = NULL;
	fputs("\ncfg :: test destruction.\n", debug_stream);
	harbol_cfg_free(&cfg);
	fprintf(debug_stream, "cfg ptr valid?: '%s'\n", cfg != NULL ? "yes" : "no");
}
