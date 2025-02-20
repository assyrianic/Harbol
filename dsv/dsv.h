#ifndef HARBOL_DSV_INCLUDED
#	define HARBOL_DSV_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"
#include "../msg_span/msg_span.h"


/**
 * Delimiter Separated Values Parser or DSV Parser.
 * 
 * Each line in a DSV file is a row and commas/whatever separates the values within a row.
 * 
 * Example of Valid Fields:
 * 
 * 1997,Ford,E350
 * 
 * "1997","Ford","E350"
 * 
 * 1997,Ford,E350,"Super, luxurious truck"
 * 
 * 1997,Ford,E350,'Super, "luxurious" truck'
 * 
 * 1997,Ford,E350,"Go get one now
they are going fast"
 * 
 * 1997, Ford, E350
 * not same as
 * 1997,Ford,E350
 * 
 * 1997, "Ford" ,E350
 */


struct HarbolDSVDB {
	char   *data; /// flattened string of all entries.
	size_t *lens; /// the string length of each entry.
	size_t *offs; /// offset of each string entry.
	size_t  size;
};


HARBOL_EXPORT NO_NULL struct HarbolDSVDB harbol_dsv_from_file(char const filename[], char const delimiter[], bool *res);
HARBOL_EXPORT NO_NULL struct HarbolDSVDB harbol_dsv_from_cstr(char const cstr[], char const delimiter[], bool *res);
HARBOL_EXPORT NO_NULL void harbol_dsv_clear(struct HarbolDSVDB *dsv_db);

HARBOL_EXPORT NO_NULL ssize_t harbol_dsv_entry_len(struct HarbolDSVDB const *dsv_db, size_t row, size_t col);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_DSV_INCLUDED */