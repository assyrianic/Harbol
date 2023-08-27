#include "msg_sys.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


static NEVER_NULL(1) void _print_file_margins(FILE *const restrict file, char const filename[const restrict], uint32_t const *const restrict line, uint32_t const *const restrict col) {
	if( filename != NULL ) {
		fprintf(file, "(%s", filename);
		if( line != NULL ) {
			fprintf(file, ":%u", *line);
		}
		if( col != NULL ) {
			fprintf(file, ":%u", *col);
		}
		fprintf(file, ") ");
	}
}

HARBOL_EXPORT void harbol_write_msg(size_t *const restrict msg_cnt, FILE *const restrict stream, char const filename[const restrict], char const msgtype[const restrict], char const msg_color[const restrict], uint32_t const *const line, uint32_t const *const col, char const msg_fmt[const restrict static 1], ...) {
	va_list args; va_start(args, msg_fmt);
	_print_file_margins(stream, filename, line, col);
	fprintf(stream, "%s%s%s: **** ", msg_color, msgtype, COLOR_RESET);
	vfprintf(stream, msg_fmt, args);
	fprintf(stream, " ****\n");
	va_end(args);
	
	if( msg_cnt != NULL ) {
		++*msg_cnt;
	}
}
