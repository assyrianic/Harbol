#ifndef HARBOL_MSG_SYS_INCLUDED
#	define HARBOL_MSG_SYS_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


#define COLOR_RED       "\x1B[31m"    /// used for errors.
#define COLOR_GREEN     "\x1B[32m"    /// used for successes.
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE      "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"    /// used for warnings.
#define COLOR_CYAN      "\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"
#define COLOR_RESET     "\033[0m"     /// used to reset the color.


HARBOL_EXPORT NEVER_NULL(2,5,8) void harbol_emit_msg_to_stream(
	size_t         *msg_cnt,        /// 1
	FILE           *stream,         /// 2
	char const      filename[],     /// 3
	char const      msgtype[],      /// 4
	char const      msg_color[],    /// 5
	uint32_t const *line,           /// 6
	uint32_t const *col,            /// 7
	char const      msg_fmt[],      /// 8
	...
);

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MSG_SYS_INCLUDED */
