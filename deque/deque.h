#ifndef HARBOL_DEQUE_INCLUDED
#	define HARBOL_DEQUE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "../harbol_common_defines.h"
#include "../harbol_common_includes.h"

struct HarbolQNode {
	uint8_t *data;
	size_t   next, prev;
};

struct HarbolDeque {
	struct HarbolQNode *nodes;
	size_t              cap, len, head, tail, freed;
};

HARBOL_EXPORT struct HarbolDeque *harbol_deque_new(size_t init_size);
HARBOL_EXPORT NO_NULL bool harbol_deque_init(struct HarbolDeque *deque, size_t init_size);
HARBOL_EXPORT NO_NULL struct HarbolDeque harbol_deque_make(size_t init_size, bool *res);

HARBOL_EXPORT NO_NULL void harbol_deque_clear(struct HarbolDeque *deque);
HARBOL_EXPORT NO_NULL void harbol_deque_free(struct HarbolDeque **dequeref);

HARBOL_EXPORT NO_NULL void harbol_deque_reset(struct HarbolDeque *deque);

HARBOL_EXPORT NO_NULL size_t harbol_deque_count(const struct HarbolDeque *deque);
HARBOL_EXPORT NO_NULL bool harbol_deque_empty(const struct HarbolDeque *deque);

HARBOL_EXPORT NO_NULL size_t harbol_deque_head(const struct HarbolDeque *deque);
HARBOL_EXPORT NO_NULL size_t harbol_deque_tail(const struct HarbolDeque *deque);

HARBOL_EXPORT NO_NULL size_t harbol_deque_next(const struct HarbolDeque *deque, size_t node);
HARBOL_EXPORT NO_NULL size_t harbol_deque_prev(const struct HarbolDeque *deque, size_t node);

HARBOL_EXPORT NO_NULL size_t harbol_deque_prepend(struct HarbolDeque *deque, const void *val, size_t datasize);
HARBOL_EXPORT NO_NULL size_t harbol_deque_append(struct HarbolDeque *deque, const void *val, size_t datasize);

HARBOL_EXPORT NO_NULL bool harbol_deque_pop_front(struct HarbolDeque *deque, void *val, size_t datasize);
HARBOL_EXPORT NO_NULL bool harbol_deque_pop_back(struct HarbolDeque *deque, void *val, size_t datasize);

HARBOL_EXPORT NO_NULL void *harbol_deque_get_front(const struct HarbolDeque *deque);
HARBOL_EXPORT NO_NULL void *harbol_deque_get_back(const struct HarbolDeque *deque);
HARBOL_EXPORT NO_NULL void *harbol_deque_get_data(const struct HarbolDeque *deque, size_t node);
/********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /** HARBOL_DEQUE_INCLUDED */
