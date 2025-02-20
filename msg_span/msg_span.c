#include "msg_span.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolTokenSpan harbol_token_span_merge(
	struct HarbolTokenSpan const span1,
	struct HarbolTokenSpan const span2
) {
	return ( struct HarbolTokenSpan ) {
		.line_start = span1.line_start > span2.line_start? span2.line_start : span1.line_start,
		.colm_start = span1.colm_start > span2.colm_start? span2.colm_start : span1.colm_start,
		.line_end   = span1.line_end   > span2.line_end?   span1.line_end   : span2.line_end,
		.colm_end   = span1.colm_end   > span2.colm_end?   span1.colm_end   : span2.colm_end,
	};
}


HARBOL_EXPORT struct HarbolMsgSpan harbol_msg_span_make(char const cstr[const restrict static 1], bool const is_filename, bool *const restrict res) {
	struct HarbolMsgSpan msgspan = {0};
	*res = harbol_msg_span_init(&msgspan, cstr, is_filename);
	return msgspan;
}

HARBOL_EXPORT bool harbol_msg_span_init(struct HarbolMsgSpan *const restrict msgspan, char const cstr[const restrict static 1], bool const is_filename) {
	(( is_filename )? harbol_string_read_file : harbol_string_copy_cstr)(&msgspan->src.code, cstr);
	if( is_filename ) {
		harbol_string_copy_cstr(&msgspan->src.filename, cstr);
	}
	lex_fix_newlines(&msgspan->src.code, true);
	
	size_t const newlines = harbol_string_count_cstr(&msgspan->src.code, "\n");
	size_t const total_lines = newlines + 1;
	if( !harbol_multi_calloc(total_lines, 0,
	                             &msgspan->src.line_starts, sizeof *msgspan->src.line_starts,
	                             &msgspan->src.line_ends, sizeof *msgspan->src.line_ends,
	                             nullptr) ) {
		harbol_string_clear(&msgspan->src.code);
		return false;
	}
	
	if( newlines > 0 ) {
		size_t *newline_offs = calloc(newlines, sizeof *newline_offs);
		if( newline_offs==nullptr ) {
			harbol_string_clear(&msgspan->src.code);
			harbol_multi_cleanup(0,
				&msgspan->src.line_starts,
				&msgspan->src.line_ends,
				nullptr);
			return false;
		}
		
		harbol_string_cstr_offsets(&msgspan->src.code, "\n", newline_offs, newlines);
		for( size_t i=0; i < newlines; i++ ) {
			size_t const offs = newline_offs[i];
			msgspan->src.line_ends[i] = ( uint32_t )(offs);
			msgspan->src.line_starts[i+1] = ( uint32_t )(offs + 1);
		}
		free(newline_offs); newline_offs = nullptr;
	}
	msgspan->src.line_ends[total_lines - 1] = msgspan->src.code.len; 
	msgspan->src.len = total_lines;
	return true;
}


static void _harbol_msg_span_purge_notes(struct HarbolMsgSpan *const msgspan, bool const free_labels) {
	for( size_t i=0; i < msgspan->notes.len; i++ ) {
		struct HarbolString *const note = harbol_array_get(&msgspan->notes, i, sizeof *note);
		harbol_string_clear(note);
	}
	if( free_labels ) {
		harbol_array_clear(&msgspan->notes);
	} else {
		harbol_array_wipe(&msgspan->notes, sizeof(struct HarbolString));
	}
}
static void _harbol_msg_span_purge_labels(struct HarbolMsgSpan *const msgspan, bool const free_labels) {
	for( size_t i=0; i < msgspan->labels.len; i++ ) {
		struct HarbolMsgLabel *const label = harbol_array_get(&msgspan->labels, i, sizeof *label);
		harbol_string_clear(&label->msg);
	}
	if( free_labels ) {
		harbol_array_clear(&msgspan->labels);
	} else {
		harbol_array_wipe(&msgspan->labels, sizeof(struct HarbolMsgLabel));
	}
}


HARBOL_EXPORT void harbol_msg_span_clear(struct HarbolMsgSpan *const msgspan) {
	harbol_string_clear(&msgspan->src.filename);
	harbol_string_clear(&msgspan->src.code);
	harbol_multi_cleanup(0,
		&msgspan->src.line_starts,
		&msgspan->src.line_ends,
		nullptr);
	_harbol_msg_span_purge_labels(msgspan, true);
	_harbol_msg_span_purge_notes(msgspan, true);
}


HARBOL_EXPORT uint32_t harbol_msg_span_line_start_offs(struct HarbolMsgSpan const *const msgspan, size_t const line) {
	return (line < msgspan->src.len)? msgspan->src.line_starts[line] : -1U;
}

HARBOL_EXPORT uint32_t harbol_msg_span_line_end_offs(struct HarbolMsgSpan const *const msgspan, size_t const line) {
	return (line < msgspan->src.len)? msgspan->src.line_ends[line] : -1U;
}

HARBOL_EXPORT NO_NULL char const *harbol_msg_span_line_start_cstr(struct HarbolMsgSpan const *const msgspan, size_t const line) {
	return (line < msgspan->src.len)? &msgspan->src.code.cstr[msgspan->src.line_starts[line]] : nullptr;
}

HARBOL_EXPORT NO_NULL char const *harbol_msg_span_line_end_cstr(struct HarbolMsgSpan const *const msgspan, size_t const line) {
	return (line < msgspan->src.len)? &msgspan->src.code.cstr[msgspan->src.line_ends[line]] : nullptr;
}

HARBOL_EXPORT size_t harbol_msg_span_get_line_len(struct HarbolMsgSpan const *const msgspan, size_t const line) {
	return (line < msgspan->src.len)? ( size_t )(msgspan->src.line_ends[line] - msgspan->src.line_starts[line]) : 0;
}


HARBOL_EXPORT struct HarbolString const *harbol_msg_span_get_code(struct HarbolMsgSpan const *const msgspan) {
	return &msgspan->src.code;
}

HARBOL_EXPORT size_t harbol_msg_span_get_num_lines(struct HarbolMsgSpan const *const msgspan) {
	return msgspan->src.len;
}


HARBOL_EXPORT bool harbol_msg_span_add_label(struct HarbolMsgSpan *const restrict msgspan, struct HarbolTokenSpan const span, char const sym_color[const restrict], int32_t const sym, char const msg_color[const restrict], char const msg[const restrict static 1], ...) {
	struct HarbolMsgLabel label = {
		.span      = span,
		.sym       = sym,
		.sym_color = sym_color,
	};
	
	if( harbol_array_full(&msgspan->labels) && !harbol_array_grow(&msgspan->labels, sizeof label) ) {
		return false;
	}
	
	va_list ap; va_start(ap, msg);
	if( msg_color != nullptr ) {
		harbol_string_add_cstr(&label.msg, msg_color);
	}
	int const format_res = harbol_string_format_va(&label.msg, false, msg, ap);
	if( msg_color != nullptr ) {
		harbol_string_add_cstr(&label.msg, COLOR_RESET);
	}
	
	bool const res = format_res >= 0 && harbol_array_insert(&msgspan->labels, &label, sizeof label);
	if( !res ) {
		harbol_string_clear(&label.msg);
	}
	return res;
}

HARBOL_EXPORT bool harbol_msg_span_add_note(struct HarbolMsgSpan *const restrict msgspan, char const msg_color[const restrict static 1], char const msg[const restrict static 1], ...) {
	struct HarbolString note = {0};
	if( harbol_array_full(&msgspan->notes) && !harbol_array_grow(&msgspan->notes, sizeof note) ) {
		return false;
	}
	
	va_list ap; va_start(ap, msg);
	if( msg_color != nullptr ) {
		harbol_string_add_cstr(&note, msg_color);
	}
	int const format_res = harbol_string_format_va(&note, false, msg, ap);
	if( msg_color != nullptr ) {
		harbol_string_add_cstr(&note, COLOR_RESET);
	}
	
	bool const res = format_res >= 0 && harbol_array_insert(&msgspan->notes, &note, sizeof note);
	if( !res ) {
		harbol_string_clear(&note);
	}
	return res;
}


static NEVER_NULL(1) void _print_file_margins(FILE *const restrict stream, char const filename[const restrict static 1], uint32_t const *const restrict line, uint32_t const *const restrict col) {
	if( filename != nullptr ) {
		fprintf(stream, "\n--> %s", filename);
		if( line != nullptr ) {
			fprintf(stream, ":%u", *line);
		}
		if( col != nullptr ) {
			fprintf(stream, ":%u", *col);
		}
	}
}


static NO_NULL void _output_span(struct HarbolMsgSpan const *const restrict msgspan, struct HarbolTokenSpan const span, size_t const len, FILE *const restrict stream) {
	char const *const restrict code_cstr = msgspan->src.code.cstr;
	for( uint32_t line = span.line_start; line <= span.line_end; line++ ) {
		struct HarbolString line_num_pad = {0};
		harbol_string_add_char_rep(&line_num_pad, ' ', len - base_10_digits(line));
		uint32_t const line_len = msgspan->src.line_ends[line-1] - msgspan->src.line_starts[line-1];
		fprintf(stream, "%u%s|%.*s\n", line, line_num_pad.cstr, ( int )(line_len), &code_cstr[msgspan->src.line_starts[line-1]]);
		harbol_string_clear(&line_num_pad);
	}
}

/// TODO: add option for line color when emitting notes.
HARBOL_EXPORT void harbol_msg_span_emit_to_stream(
	struct HarbolMsgSpan *const restrict       msgspan,
	size_t *const restrict                     msg_cnt,
	FILE *const restrict                       stream,
	char const                                 filename[const restrict],
	char const                                 msgtype[const restrict],
	char const                                 code_num[const restrict],
	char const                                 msgtype_color[const restrict static 1],
	uint32_t const *const restrict             file_line,
	uint32_t const *const restrict             file_col,
	char const                                 msg_color[const restrict],
	char const                                 msg_fmt[const restrict static 1],
	...
) {
	if( msgtype != nullptr ) {
		fprintf(stream, "%s%s", msgtype_color, msgtype);
		if( code_num != nullptr ) {
			fprintf(stream, "[%s]", code_num);
		}
		fprintf(stream, "%s: ", COLOR_RESET);
	}
	va_list args; va_start(args, msg_fmt);
	if( msg_color != nullptr ) {
		fprintf(stream, "%s", msg_color);
	}
	vfprintf(stream, msg_fmt, args);
	fprintf(stream, "%s", COLOR_RESET);
	_print_file_margins(stream, filename, file_line, file_col);
	
	if( msg_cnt != nullptr ) {
		++*msg_cnt;
	}
	
	if( msgspan->labels.len > 0 ) {
		fprintf(stream, "\n");
		uint32_t largest_span = 0;
		struct HarbolMsgLabel const *labels = ( struct HarbolMsgLabel const* )(msgspan->labels.table);
		for( size_t i=0; i < msgspan->labels.len; i++ ) {
			if( largest_span < labels[i].span.line_end ) {
				largest_span = labels[i].span.line_end;
			}
		}
		
		/// Using log10 + 1 gives us how many digits a decimal value has.
		struct HarbolString span_pad = {0};
		harbol_string_add_char_rep(&span_pad, ' ', (base_10_digits(largest_span) + 1));
		fprintf(stream, "%s|\n", span_pad.cstr);
		
		for( size_t i=0; i < msgspan->labels.len; i++ ) {
			struct HarbolTokenSpan const span = labels[i].span;
			
			/// here we emit the actual code line.
			_output_span(msgspan, span, span_pad.len, stream);
			
			/// here we emit the label msgs.
			struct HarbolString colm_pad={0}, hilighter={0};
			harbol_string_add_char_rep(&colm_pad, ' ', span.colm_start);
			for( size_t n=0; n < (span.colm_end - span.colm_start); n++ ) {
				write_utf8_str(&hilighter, labels[i].sym);
			}
			
			char const *const restrict sym_color = labels[i].sym_color;
			fprintf(stream, "%s|%s%s%s%s %s\n", span_pad.cstr, colm_pad.cstr, sym_color==nullptr? "" : sym_color, hilighter.cstr, COLOR_RESET, labels[i].msg.cstr);
			harbol_string_clear(&colm_pad);
			harbol_string_clear(&hilighter);
		}
		_harbol_msg_span_purge_labels(msgspan, false);
		
		if( msgspan->notes.len > 0 ) {
			fprintf(stream, "%s|\n", span_pad.cstr);
			struct HarbolString const *notes = ( struct HarbolString const* )(msgspan->notes.table);
			for( size_t i=0; i < msgspan->notes.len; i++ ) {
				fprintf(stream, "%s%s\n", span_pad.cstr, notes[i].cstr);
			}
			_harbol_msg_span_purge_notes(msgspan, false);
		}
		harbol_string_clear(&span_pad);
	}
}
