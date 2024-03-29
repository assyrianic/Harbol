#include "bytebuffer.h"

#ifdef OS_WINDOWS
#	define HARBOL_LIB
#endif


HARBOL_EXPORT struct HarbolByteBuf *harbol_bytebuffer_new(void) {
	return calloc(1, sizeof(struct HarbolByteBuf));
}

HARBOL_EXPORT struct HarbolByteBuf harbol_bytebuffer_make(void) {
	struct HarbolByteBuf buf = {0};
	return buf;
}

HARBOL_EXPORT void harbol_bytebuffer_clear(struct HarbolByteBuf *const buf) {
	free(buf->table); buf->table = NULL;
	*buf = (struct HarbolByteBuf){0};
}

HARBOL_EXPORT void harbol_bytebuffer_free(struct HarbolByteBuf **const buf_ref) {
	if( *buf_ref==NULL ) {
		return;
	}
	harbol_bytebuffer_clear(*buf_ref);
	free(*buf_ref); *buf_ref = NULL;
}

HARBOL_EXPORT size_t harbol_bytebuffer_cap(struct HarbolByteBuf const *const buf) {
	return buf->cap;
}

HARBOL_EXPORT size_t harbol_bytebuffer_len(struct HarbolByteBuf const *const buf) {
	return buf->len;
}

HARBOL_EXPORT uint8_t *harbol_bytebuffer_get_buffer(struct HarbolByteBuf const *const buf) {
	return buf->table;
}

static NO_NULL bool _harbol_buffer_resize(struct HarbolByteBuf *const restrict buf, size_t const new_size) {
	uint8_t *const new_table = harbol_recalloc(buf->table, new_size, sizeof *buf->table, buf->cap);
	if( new_table==NULL ) {
		return false;
	}
	buf->table = new_table;
	buf->cap   = new_size;
	return true;
}

#ifndef HARBOL_BYTEBUFFER_INSERTION
#	define HARBOL_BYTEBUFFER_INSERTION \
	if( buf->len + sizeof val >= buf->cap && !_harbol_buffer_resize(buf, buf->len + sizeof val) ) \
		return false; \
	\
	memcpy(&buf->table[buf->len], &val, sizeof val); \
	buf->len += sizeof val; \
	return true;
#endif

HARBOL_EXPORT bool harbol_bytebuffer_insert_byte(struct HarbolByteBuf *const buf, uint8_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_int16(struct HarbolByteBuf *const buf, uint16_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_int32(struct HarbolByteBuf *const buf, uint32_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_int64(struct HarbolByteBuf *const buf, uint64_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_ptr(struct HarbolByteBuf *const buf, uintptr_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_float32(struct HarbolByteBuf *const buf, float32_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_float64(struct HarbolByteBuf *const buf, float64_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_floatmax(struct HarbolByteBuf *const buf, floatmax_t const val) {
	HARBOL_BYTEBUFFER_INSERTION
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_cstr(struct HarbolByteBuf *const restrict buf, char const cstr[static 1]) {
	size_t const cstr_len = strlen(cstr);
	if( buf->len + cstr_len + 1 >= buf->cap && !_harbol_buffer_resize(buf, buf->len + cstr_len + 1) ) {
		return false;
	}
	strcpy(( char* )(&buf->table[buf->len]), cstr);
	buf->len += cstr_len;
	buf->table[buf->len++] = 0;
	return true;
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_obj(struct HarbolByteBuf *const restrict buf, void const *const obj, size_t const size) {
	if( buf->len + size >= buf->cap && !_harbol_buffer_resize(buf, buf->len + size) ) {
		return false;
	}
	memcpy(&buf->table[buf->len], obj, size);
	buf->len += size;
	return true;
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_zeros(struct HarbolByteBuf *const buf, size_t const amount) {
	if( buf->len + amount >= buf->cap && !_harbol_buffer_resize(buf, buf->len + amount) ) {
		return false;
	}
	memset(&buf->table[buf->len], 0, amount);
	buf->len += amount;
	return true;
}

HARBOL_EXPORT bool harbol_bytebuffer_del(struct HarbolByteBuf *const buf, size_t const index, size_t const range) {
	return array_shift_up(buf->table, &buf->len, index, sizeof *buf->table, range);
}

HARBOL_EXPORT bool harbol_bytebuffer_to_file(struct HarbolByteBuf const *const buf, FILE *const file) {
	if( buf->table==NULL ) {
		return false;
	}
	size_t const bytes_written = fwrite(buf->table, sizeof *buf->table, buf->len, file);
	return bytes_written==buf->len;
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_from_filename(struct HarbolByteBuf *const restrict buf, char const filename[static 1]) {
	FILE *restrict file = fopen(filename, "r");
	if( file==NULL ) {
		return false;
	}
	
	bool const res = harbol_bytebuffer_insert_from_file(buf, file);
	fclose(file);
	return res;
}

HARBOL_EXPORT bool harbol_bytebuffer_insert_from_file(struct HarbolByteBuf *const buf, FILE *const file) {
	fseek(file, 0, SEEK_END);
	long const file_size = ftell(file);
	if( file_size <= 0 ) {
		return false;
	}
	
	rewind(file);
	if( buf->len + file_size >= buf->cap && !_harbol_buffer_resize(buf, buf->len + file_size) ) {
		return false;
	}
	size_t const bytes_read = fread(&buf->table[buf->len], sizeof *buf->table, file_size, file);
	buf->len += bytes_read;
	return bytes_read==( size_t )(file_size);
}

HARBOL_EXPORT bool harbol_bytebuffer_append(struct HarbolByteBuf *const bufA, struct HarbolByteBuf const *const bufB) {
	if( bufB->table==NULL || (bufA->len + bufB->len >= bufA->cap && !_harbol_buffer_resize(bufA, bufA->len + bufB->len)) ) {
		return false;
	}
	memcpy(&bufA->table[bufA->len], bufB->table, bufB->len);
	bufA->len += bufB->len;
	return true;
}

HARBOL_EXPORT bool harbol_bytebuffer_copy(struct HarbolByteBuf *const bufA, struct HarbolByteBuf const *const bufB) {
	if( bufB->table==NULL || (bufB->len != bufA->len && !_harbol_buffer_resize(bufA, bufB->len)) ) {
		return false;
	}
	memcpy(&bufA->table[0], &bufB->table[0], bufB->len);
	bufA->len = bufB->len;
	return true;
}
