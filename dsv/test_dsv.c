#include <assert.h>
#include <stdalign.h>
#include <time.h>
#include "dsv.h"

void test_harbol_dsv(FILE *debug_stream);

#ifdef HARBOL_USE_MEMPOOL
struct HarbolMemPool *g_pool;
#endif

union Value {
	int64_t int64;
};

int main(void) {
	FILE *debug_stream = fopen("harbol_dsv_output.txt", "w");
	if( debug_stream==nullptr )
		return -1;
	
#ifdef HARBOL_USE_MEMPOOL
	struct HarbolMemPool m = harbol_mempool_make(10000000, 32, LVAL_PTR(bool, false));
	g_pool = &m;
#endif
	test_harbol_dsv(debug_stream);
	
	fclose(debug_stream); debug_stream=nullptr;
#ifdef HARBOL_USE_MEMPOOL
	harbol_mempool_clear(g_pool, false);
#endif
}


void test_harbol_dsv(FILE *const debug_stream) {
	/// Test allocation and initializations
	fputs("dsv :: test allocation/initialization.\n", debug_stream);
	
	char const test_cstr[] = "test1,row1,bigkeks\nd,e,f\ng,h,i";
	struct HarbolDSVDB dsv_db = harbol_dsv_from_cstr(test_cstr, ",", LVAL_PTR(bool, false));
	
	harbol_dsv_clear(&dsv_db);
}

/*
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

size_t csubstr_diff(char const cstr[], char const csubstr[], char const end[]) {
	char const *const pos = strstr(cstr, csubstr);
	return( pos==NULL || (end != NULL && pos >= end) )? SIZE_MAX : ( size_t )(pos - cstr);
}

size_t harbol_count_subcstr(char const cstr[], char const subcstr[], char const end[]) {
	size_t occurrences = 0;
	size_t const occ_len = strlen(subcstr);
	for( char const *pos = strstr(cstr, subcstr); pos != NULL && (end==NULL || pos < end); pos = strstr(pos + occ_len, subcstr) ) {
		occurrences++;
	}
	return occurrences;
}

size_t harbol_cstr_replace_substr_len(
	size_t const cstr_len,    char const cstr[],
	size_t const replace_len, char const replace_cstr[],
	size_t const with_len,
	size_t *const restrict amount
) {
	size_t const counts = harbol_count_subcstr(cstr, replace_cstr, NULL);
	if( counts==0 ) {
		return 0;
	}
	if( *amount > counts ) {
		*amount = counts;
	}
	return cstr_len + (*amount * (with_len - replace_len));
}

char *harbol_cstr_replace(
	size_t *const restrict cstr_len, char cstr[],
	size_t const replace_len, char const replace_cstr[],
	size_t const with_len,    char const with_cstr[],
	size_t amount
) {
	size_t const len_calc = harbol_cstr_replace_substr_len(*cstr_len, cstr, replace_len, replace_cstr, with_len, &amount);
	if( len_calc==0 ) {
		return NULL;
	}
	*cstr_len = len_calc;
	size_t offset = csubstr_diff(&cstr[0], replace_cstr, NULL);
	size_t rep_len = offset;
	/// if the substring we're replacing is larger than the replacer, we don't need to allocate.
	/// we can just replace that string and shift up the string.
	if( with_len <= replace_len ) {
		memcpy(&cstr[rep_len], with_cstr, with_len);
		rep_len += with_len;
		memmove(&cstr[rep_len], &cstr[rep_len + replace_len], replace_len-with_len);
		/*
		for( size_t i=0; i < amount; i++ ) {
			size_t const saved_offset = offset;
			size_t const relative_offs = csubstr_diff(&cstr[saved_offset], replace_cstr, NULL);
			if( relative_offs==SIZE_MAX ) {
				break;
			}
			offset += relative_offs;
			size_t const span = offset - saved_offset;
			memmove(&cstr[rep_len], &cstr[saved_offset], span);
			rep_len += span;
			
			memcpy(&cstr[rep_len], with_cstr, with_len);
			rep_len += with_len;
			offset += replace_len;
		}
		//*
		return cstr;
	} else {
		char *restrict new_cstr = calloc(len_calc + 1, sizeof *new_cstr);
		if( new_cstr==NULL ) {
			return NULL;
		}
		/// first copy contents up to the first offset.
		strncpy(&new_cstr[0], &cstr[0], offset);
		offset += replace_len;
		
		strcpy(&new_cstr[rep_len], with_cstr);
		rep_len += with_len;
		for( size_t i=0; i < amount; i++ ) {
			size_t const saved_offset = offset;
			size_t const relative_offs = csubstr_diff(&cstr[saved_offset], replace_cstr, NULL);
			if( relative_offs==SIZE_MAX ) {
				break;
			}
			
			offset += relative_offs;
			size_t const span = offset - saved_offset;
			strncpy(&new_cstr[rep_len], &cstr[saved_offset], span);
			rep_len += span;
			
			strcpy(&new_cstr[rep_len], with_cstr);
			rep_len += with_len;
			offset += replace_len;
		}
		strcpy(&new_cstr[rep_len], &cstr[offset]);
		rep_len += offset;
		return new_cstr;
	}
}

int main() {
    char my_str[] = "We believe in one God, the Father Almighty, Maker of heaven and earth, and of all things visible and invisible. And in one Lord Jesus Christ, the only-begotten Son of God, begotten of the Father before all worlds (æons), Light of Light, very God of very God, begotten, not made, consubstantial with the Father; by whom all things were made; who for us men, and for our salvation, came down from heaven, and was incarnate by the Holy Ghost and of the Virgin Mary, and was made man; he was crucified for us under Pontius Pilate, and suffered, and was buried, and the third day he rose again, according to the Scriptures, and ascended into heaven, and sitteth on the right hand of the Father; from thence he shall come again, with glory, to judge the quick and the dead; whose kingdom shall have no end. And in the Holy Ghost, the Lord and Giver of life, who proceedeth from the Father, who with the Father and the Son together is worshiped and glorified, who spake by the prophets. In one holy catholic and apostolic Church; we acknowledge one baptism for the remission of sins; we look for the resurrection of the dead, and the life of the world to come. Amen.";
    char const to_replace[] = "and";
    char const replace_with[] = "&";
    size_t cstr_len = (sizeof my_str) - 1;
    printf("old cstr_len: %zu | replace_len: %zu | with_len: %zu\n", cstr_len, sizeof to_replace-1, sizeof replace_with-1);
    char *s = harbol_cstr_replace(&cstr_len, my_str, (sizeof to_replace) - 1, to_replace, (sizeof replace_with) - 1, replace_with, -1);
    printf("new cstr_len: %zu | new str: '%s'\n", cstr_len, s);
    if( s != &my_str[0] ) {
        puts("====freed s");
        free(s); s = NULL;
    }
}
*/