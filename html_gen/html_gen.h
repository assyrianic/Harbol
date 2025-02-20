#ifndef HARBOL_HTML_GEN_INCLUDED
#	define HARBOL_HTML_GEN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"


/// See
/// https://developer.mozilla.org/en-US/docs/Learn/HTML/Introduction_to_HTML/Document_and_website_structure
struct HarbolHTMLGen {
	struct HarbolHTMLHeader  *header;
	struct HarbolHTMLNavBar  *navbar;
	struct HarbolHTMLSideBar *sidebar;
	struct HarbolHTMLFooter  *footer;
};

HARBOL_EXPORT NO_NULL bool harbol_htmlgen_init(struct HarbolHTMLGen *html);
HARBOL_EXPORT NO_NULL struct HarbolHTMLGen harbol_htmlgen_make(bool *res);

/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_HTML_GEN_INCLUDED */