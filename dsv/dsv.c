#include "dsv.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


#define HARBOL_DSV_DEFAULT_DELIM    ","

bool is_delim(int32_t c, void *data) {
	char const *const restrict t = data;
	for( size_t i=0; t[i] != 0; ) {
		int32_t const rune = utf8_to_rune_iter_no_len(t, &i);
		if( rune==c ) {
			return false;
		}
	}
	return true;
}

static NO_NULL bool _parse_dsv(char const dsv[const restrict static 1], char const delimiter[const restrict static 1], struct HarbolDSVDB *const restrict dsv_db) {
	bool msg_span_res = false;
	
	bool parsing_res = false;
	struct HarbolMsgSpan msg_span = harbol_msg_span_make(dsv, false, &msg_span_res);
	if( !msg_span_res ) {
		goto parse_dsv_err;
	}
	
	/// the number of entries in the first row defines how many columns each entry requires.
	size_t const num_rows = harbol_msg_span_get_num_lines(&msg_span);
	if( num_rows==0 ) {
		goto parse_dsv_err;
	}
	
	size_t total_cstr_size = 0;
	char const *curr_line_start = harbol_msg_span_line_start_cstr(&msg_span, 0);
	char const *curr_line_end = harbol_msg_span_line_end_cstr(&msg_span, 0);
	size_t line_len = harbol_msg_span_get_line_len(&msg_span, 0);
	size_t const num_cols = harbol_count_subcstr(curr_line_start, delimiter, curr_line_end) + 1;
	dsv_db->size = num_cols * num_rows;
	if( !harbol_multi_calloc(num_rows * num_cols, 0,
	                         &dsv_db->offs, sizeof *dsv_db->offs,
	                         &dsv_db->lens, sizeof *dsv_db->lens,
	                         nullptr) ) {
		goto parse_dsv_err;
	}
	
	size_t total_entries = 0;
	size_t const delimiter_len = strlen(delimiter);
	for( size_t line_idx=0; line_idx < num_rows; line_idx++ ) {
		curr_line_start = harbol_msg_span_line_start_cstr(&msg_span, line_idx);
		curr_line_end = harbol_msg_span_line_end_cstr(&msg_span, line_idx);
		line_len = harbol_msg_span_get_line_len(&msg_span, line_idx);
		//printf("_parse_dsv :: current line '" HARBOL_SUBCSTR_FMT "' | delimiter_len: '%zu' | line_idx: '%zu'\n", (int)(line_len), curr_line_start, delimiter_len, line_idx);
		
		size_t entry_idx = 0;
		size_t num_entries = 0;
		while( entry_idx < line_len && num_entries < num_cols ) {
			if( curr_line_start[entry_idx]=='"' || curr_line_start[entry_idx]=='\'' ) {
				/// handle quoted entry.
				line_idx++;
				continue;
			}
			size_t const old_entry_idx = entry_idx;
			entry_idx += harbol_csubstr_offset_to_end(curr_line_start, old_entry_idx, delimiter, curr_line_end);
			
			size_t const entry_len = entry_idx - old_entry_idx;
			total_cstr_size += entry_len;
			printf("_parse_dsv :: row#(%zu) - entry: '%.*s' | entry_len: %zu\n", line_idx+1, (int)(entry_len), &curr_line_start[old_entry_idx], entry_len);
			entry_idx += delimiter_len;
			num_entries++;
		} /// while
		total_entries += num_entries;
	} /// for
	printf("_parse_dsv :: total_cstr_size: %zu\n", total_cstr_size);
	printf("_parse_dsv :: total_entries: %zu\n", total_entries);
	printf("_parse_dsv :: total bytes:: %zu\n", total_cstr_size + total_entries);
	
	parsing_res = true;
parse_dsv_err:;
	harbol_msg_span_clear(&msg_span);
	return parsing_res;
}

HARBOL_EXPORT struct HarbolDSVDB harbol_dsv_from_file(char const filename[const restrict static 1], char const delimiter[const restrict static 1], bool *const restrict res) {
	size_t text_len = 0;
	char *restrict text = make_buffer_from_text(filename, &text_len);
	if( text==nullptr || text_len==0 ) {
		*res = false;
		return ( struct HarbolDSVDB ){0};
	}
	return harbol_dsv_from_cstr(text, delimiter, res);
}

HARBOL_EXPORT NO_NULL struct HarbolDSVDB harbol_dsv_from_cstr(char const cstr[const restrict static 1], char const delimiter[const restrict static 1], bool *const restrict res) {
	struct HarbolDSVDB dsv_db = {0};
	*res = _parse_dsv(cstr, delimiter, &dsv_db);
	return dsv_db;
}

HARBOL_EXPORT void harbol_dsv_clear(struct HarbolDSVDB *const dsv_db) {
	harbol_multi_cleanup(0,
		&dsv_db->data,
		&dsv_db->lens,
		&dsv_db->offs,
		nullptr);
	dsv_db->size = 0;
}