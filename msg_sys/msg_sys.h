#ifndef HARBOL_MSG_SYS_INCLUDED
#	define HARBOL_MSG_SYS_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


#define COLOR_RED       "\x1B[31m"    /// used for errors.
#define COLOR_MAGENTA   "\x1B[35m"    /// used for warnings.
#define COLOR_RESET     "\033[0m"     /// used to reset the color.


HARBOL_EXPORT NEVER_NULL(6) void harbol_err_msg(size_t *err_count, const char filename[], const char errtype[], const size_t *line, const size_t *col, const char err[], ...);

HARBOL_EXPORT NEVER_NULL(6) void harbol_warn_msg(size_t *warn_count, const char filename[], const char warntype[], const size_t *line, const size_t *col, const char warn[], ...);

HARBOL_EXPORT NEVER_NULL(7) void harbol_log_err(FILE *file, size_t *err_count, const char filename[], const char errtype[], const size_t *line, const size_t *col, const char err[], ...);

HARBOL_EXPORT NEVER_NULL(7) void harbol_log_warn(FILE *file, size_t *warn_count, const char filename[], const char warntype[], const size_t *line, const size_t *col, const char warn[], ...);

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MSG_SYS_INCLUDED */
