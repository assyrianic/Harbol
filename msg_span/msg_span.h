#ifndef HARBOL_MSG_SPAN_INCLUDED
#	define HARBOL_MSG_SPAN_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

#include "../str/str.h"
#include "../array/array.h"
#include "../lex/lex.h"


#define COLOR_RED       "\x1B[31m"    /// used for errors.
#define COLOR_GREEN     "\x1B[32m"    /// used for successes.
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE      "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"    /// used for warnings.
#define COLOR_CYAN      "\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"
#define COLOR_RESET     "\033[0m"     /// used to reset the color.


struct HarbolTokenSpan {
	uint32_t
		line_start,
		line_end,
		colm_start,
		colm_end
	;
};

HARBOL_EXPORT struct HarbolTokenSpan harbol_token_span_merge(struct HarbolTokenSpan span1, struct HarbolTokenSpan span2);


struct HarbolMsgLabel {
	struct HarbolString    msg;
	struct HarbolTokenSpan span;
	char const            *sym_color;
	int32_t                sym;
};

struct HarbolSrcSpan {
	struct HarbolString filename, code, *lines;
	size_t              len;
};

struct HarbolMsgSpan {
	//struct HarbolMap     files; /// map[string]HarbolSrcSpan
	struct HarbolSrcSpan src;
	struct HarbolArray   labels, notes;
};


HARBOL_EXPORT NO_NULL struct HarbolMsgSpan harbol_msg_span_make(char const cstr[], bool is_filename, bool free_src_str, bool *res);
HARBOL_EXPORT NO_NULL bool harbol_msg_span_init(struct HarbolMsgSpan *msgspan, char const cstr[], bool is_filename, bool free_src_str);
HARBOL_EXPORT NO_NULL void harbol_msg_span_clear(struct HarbolMsgSpan *msgspan);


HARBOL_EXPORT NO_NULL struct HarbolString const *harbol_msg_span_get_line(struct HarbolMsgSpan const *msgspan, size_t line);

HARBOL_EXPORT NO_NULL struct HarbolString const *harbol_msg_span_get_code(struct HarbolMsgSpan const *msgspan);
HARBOL_EXPORT NO_NULL size_t harbol_msg_span_get_num_lines(struct HarbolMsgSpan const *msgspan);

HARBOL_EXPORT NEVER_NULL(1,6) bool harbol_msg_span_add_label(struct HarbolMsgSpan *msgspan, struct HarbolTokenSpan span, char const sym_color[], int32_t sym, char const msg_color[], char const msg[], ...);

HARBOL_EXPORT NEVER_NULL(1,3) bool harbol_msg_span_add_note(struct HarbolMsgSpan *msgspan, char const msg_color[], char const msg[], ...);


HARBOL_EXPORT NEVER_NULL(1,3,7,10) void harbol_msg_span_emit_to_stream(
	struct HarbolMsgSpan       *msgspan,
	size_t                     *msg_cnt,
	FILE                       *output,
	char const                  filename[],
	char const                  msgtype[],
	char const                  code_num[],
	char const                  msgtype_color[],
	uint32_t const             *line,
	uint32_t const             *col,
	char const                  msg_fmt[],
	...
);

#ifdef __cplusplus
}
#endif

#endif /** HARBOL_MSG_SPAN_INCLUDED */