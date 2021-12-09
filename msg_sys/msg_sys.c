#include "msg_sys.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


static NEVER_NULL(1) void _print_file_margins(FILE *const restrict file, const char filename[], const size_t *const line, const size_t *const col) {
	if( filename != NULL ) {
		fprintf(file, "(%s", filename);
		if( line != NULL )
			fprintf(file, ":%zu", *line);
		if( col != NULL )
			fprintf(file, ":%zu", *col);
		fprintf(file, ") ");
	}
}

HARBOL_EXPORT void harbol_err_msg(size_t *const restrict err_cnt, const char filename[], const char errtype[], const size_t *const line, const size_t *const col, const char err[], ...)
{
	va_list args; va_start(args, err);
	_print_file_margins(stderr, filename, line, col);
	fprintf(stderr, "%s%s%s: **** ", COLOR_RED, errtype, COLOR_RESET);
	vfprintf(stderr, err, args);
	fprintf(stderr, " ****\n");
	va_end(args);
	
	if( err_cnt != NULL )
		++*err_cnt;
}

HARBOL_EXPORT void harbol_warn_msg(size_t *const restrict warn_cnt, const char filename[], const char warntype[], const size_t *const line, const size_t *const col, const char warn[], ...)
{
	va_list args; va_start(args, warn);
	_print_file_margins(stderr, filename, line, col);
	
	fprintf(stderr, "%s%s%s: **** ", COLOR_MAGENTA, warntype, COLOR_RESET);
	vfprintf(stderr, warn, args);
	fprintf(stderr, " ****\n");
	va_end(args);
	
	if( warn_cnt != NULL )
		++*warn_cnt;
}


HARBOL_EXPORT void harbol_log_err(FILE *const restrict file, size_t *const restrict err_cnt, const char filename[static 1], const char errtype[static 1], const size_t *const line, const size_t *const col, const char err[static 1], ...)
{
	va_list args; va_start(args, err);
	_print_file_margins(file, filename, line, col);
	
	fprintf(file, "%s: **** ", errtype);
	vfprintf(file, err, args);
	fprintf(file, " ****\n");
	va_end(args);
	
	if( err_cnt != NULL )
		++*err_cnt;
}

HARBOL_EXPORT void harbol_log_warn(FILE *const restrict file, size_t *const restrict warn_cnt, const char filename[static 1], const char warntype[static 1], const size_t *const line, const size_t *const col, const char warn[static 1], ...)
{
	va_list args; va_start(args, warn);
	_print_file_margins(file, filename, line, col);
	
	fprintf(file, "%s: **** ", warntype);
	vfprintf(file, warn, args);
	fprintf(file, " ****\n");
	va_end(args);
	
	if( warn_cnt != NULL )
		++*warn_cnt;
}
