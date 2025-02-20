#ifndef HARBOL_BUFOPS_INCLUDED
#	define HARBOL_BUFOPS_INCLUDED


HELPER_FUNC NO_NULL void *harbol_mempcpy(void *const dest, void const *const src, size_t const bytes) {
	uint8_t *const r = C_PTR_CAST(r, memcpy(dest, src, bytes));
	return r + bytes;
}

#ifdef C23
#	ifdef harbol_memccpy
#		undef harbol_memccpy
#	endif
#	define harbol_memccpy(dest, src, c, count)    memccpy((dest), (src), (c), (count))
#else
HELPER_FUNC NO_NULL void *harbol_memccpy(void *const restrict dest, void const *const src, int const c, size_t const bytes) {
	uint8_t const *const p = C_PTR_CAST(p, memchr(src, c, bytes));
	if( p != nullptr ) {
		uint8_t const *const s = C_PTR_CAST(s, src);
		return harbol_mempcpy(dest, src, (p - s + 1));
	}
	memcpy(dest, src, bytes);
	return nullptr;
}
#endif


HELPER_FUNC uint8_t *harbol_resize_cstr(uint8_t ARRAY_PARAM_REF(cstr), size_t const bytes, size_t *const restrict len, size_t const new_size) {
	uint8_t *const restrict new_cstr = harbol_recalloc(cstr, new_size + 1, bytes, *len);
	if( new_cstr==nullptr ) {
		return nullptr;
	}
	*len = new_size;
	return new_cstr;
}


/// END the params with a nullptr.
/// Within a variable list of strings to compare to,
/// returns an index or SIZE_MAX aka (size_t)(-1) if not found
HELPER_FUNC size_t cstr_switch(char const ARRAY_PARAM_LIM(cstr, 1), ...) {
	va_list ap; va_start(ap, cstr);
	size_t index = 0;
	size_t found = SIZE_MAX;
	for( char const *arg = va_arg(ap, char const*); arg != nullptr; arg = va_arg(ap, char const*) ) {
		if( !strcmp(cstr, arg) ) {
			found = index;
			break;
		}
		index++;
	}
	va_end(ap);
	return found;
}


/// if the shift amount exceeds the length of the array, zero everything after the index and lower the count.
HELPER_FUNC NO_NULL bool harbol_buffer_shift_up(void *const buf, size_t *const restrict len, size_t const index, size_t const datasize, size_t amount) {
	if( index >= *len || datasize==0 ) {
		return false;
	}
	amount += amount==0;
	size_t const shift_idx = index + amount;
	if( shift_idx < *len ) {
		uint8_t *const b = buf;
		*len -= amount;
		memmove(&b[index * datasize], &b[shift_idx * datasize], (*len - index) * datasize);
		memset(&b[*len * datasize], 0, amount * datasize);
	} else {
		uint8_t *const restrict b = buf;
		memset(&b[index * datasize], 0, (*len - index) * datasize);
		*len = index;
	}
	return true;
}


/// This is for shifting multiple buffers that are controlled by a single length size.
/// use like: 'harbol_multi_buffer_shift_up(&len, index_to_shift_from, amount_to_shift, num_buffers, buf1, sizeof *buf1, ...);'
/// perfect for use with parallel arrays/buffers.
HELPER_FUNC bool harbol_multi_buffer_shift_up(size_t *const restrict len, size_t const index, size_t amount, size_t n_buffers, ...) {
	if( index >= *len ) {
		return false;
	}
	
	va_list ap; va_start(ap, n_buffers);
	if( n_buffers==0 ) {
		n_buffers = _harbol_count_va_buffers(ap);
		va_start(ap, n_buffers);
	}
	
	amount += amount==0;
	size_t const shift_idx = index + amount;
	if( shift_idx < *len ) {
		*len -= amount;
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const buf      = va_arg(ap, uint8_t*);
			size_t   const datasize = va_arg(ap, size_t);
			if( datasize==0 ) {
				continue;
			}
			memmove(&buf[index * datasize], &buf[shift_idx * datasize], (*len - index) * datasize);
			memset(&buf[*len * datasize], 0, amount * datasize);
		}
	} else {
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const restrict buf      = va_arg(ap, uint8_t*);
			size_t   const          datasize = va_arg(ap, size_t);
			if( datasize==0 ) {
				continue;
			}
			memset(&buf[index * datasize], 0, (*len - index) * datasize);
		}
		*len = index;
	}
	va_end(ap);
	return true;
}

HELPER_FUNC bool harbol_shift_up_buffers(size_t *const restrict len, size_t const index, size_t amount, size_t const n_buffers, void *buffers[const restrict static n_buffers], size_t const elem_sizes[const static n_buffers]) {
	if( index >= *len ) {
		return false;
	}
	
	amount += !amount;
	size_t const i = index + amount;
	if( i < *len ) {
		*len -= amount;
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const buf = buffers[n];
			size_t const datasize = elem_sizes[n];
			if( datasize==0 ) {
				continue;
			}
			memmove(&buf[index * datasize], &buf[i * datasize], (*len - index) * datasize);
			memset(&buf[*len * datasize], 0, amount * datasize);
		}
	} else {
		for( size_t n=0; n < n_buffers; n++ ) {
			uint8_t *const restrict buf = buffers[n];
			size_t const datasize = elem_sizes[n];
			if( datasize==0 ) {
				continue;
			}
			memset(&buf[index * datasize], 0, (*len - index) * datasize);
		}
		*len = index;
	}
	return true;
}

HELPER_FUNC NEVER_NULL(1,2) size_t harbol_count_subcstr(char const ARRAY_PARAM_FULL(cstr, 1), char const ARRAY_PARAM_FULL(subcstr, 1), char const end[]) {
	size_t occurrences = 0;
	size_t const occ_len = strlen(subcstr);
	for( char const *pos = strstr(cstr, subcstr); pos != nullptr && (end==nullptr || pos < end); pos = strstr(pos + occ_len, subcstr) ) {
		occurrences++;
	}
	return occurrences;
}

HELPER_FUNC NEVER_NULL(1,3,4) bool harbol_csubstr_offset(char const ARRAY_PARAM_FULL(cstr,1), size_t const start, char const ARRAY_PARAM_FULL(subcstr,1), size_t *const restrict offset, char const end[]) {
	size_t const diff = csubstr_diff(&cstr[start], subcstr, end);
	if( diff==SIZE_MAX ) {
		return false;
	}
	*offset += diff;
	return true;
}

HELPER_FUNC NEVER_NULL(1,2,5) size_t harbol_cstr_offsets(char const ARRAY_PARAM_FULL(cstr,1), char const ARRAY_PARAM_FULL(subcstr,1), char const end[], size_t const offsets_len, size_t ARRAY_PARAM_FULL(offsets, offsets_len)) {
	size_t offset = 0, counts = 0;
	for( size_t i=0; i < offsets_len; i++ ) {
		size_t const curr_offs = offset;
		if( !harbol_csubstr_offset(cstr, curr_offs, subcstr, &offset, end) ) {
			break;
		}
		offsets[i] = offset;
		offset++;
		counts++;
	}
	return counts;
}

HELPER_FUNC NO_NULL size_t harbol_csubstr_offset_to_end(char const ARRAY_PARAM_LIM(cstr,1), size_t const start, char const ARRAY_PARAM_FULL(subcstr,1), char const ARRAY_PARAM_LIM(end, 1)) {
	char const *const pos = strstr(&cstr[start], subcstr);
	return ( size_t )( ((pos==nullptr || pos > end)? end : pos) - &cstr[start] );
}

HELPER_FUNC NO_NULL size_t harbol_cstr_replace_substr_len(
	size_t const cstr_len,    char const ARRAY_PARAM_FULL(cstr, cstr_len),
	size_t const replace_len, char const ARRAY_PARAM_FULL(replace_cstr,replace_len),
	size_t const with_len,
	size_t *const restrict amount
) {
	size_t const counts = harbol_count_subcstr(cstr, replace_cstr, nullptr);
	if( counts==0 ) {
		return 0;
	}
	if( *amount > counts ) {
		*amount = counts;
	}
	return cstr_len + (*amount * (with_len - replace_len));
}

HELPER_FUNC NO_NULL char *harbol_cstr_replace(
	size_t *const restrict cstr_len, char ARRAY_PARAM_LIM(cstr, 1),
	size_t const replace_len, char const ARRAY_PARAM_FULL(replace_cstr,replace_len),
	size_t const with_len,    char const ARRAY_PARAM_FULL(with_cstr,with_len),
	size_t amount
) {
	size_t const len_calc = harbol_cstr_replace_substr_len(*cstr_len, cstr, replace_len, replace_cstr, with_len, &amount);
	if( len_calc==0 ) {
		return nullptr;
	}
	*cstr_len = len_calc;
	size_t offset = csubstr_diff(&cstr[0], replace_cstr, nullptr);
	size_t rep_len = offset;
	/// if the substring we're replacing is larger than the replacer, we don't need to allocate.
	/// we can just replace that string and shift up the string.
	if( with_len <= replace_len ) {
		strcpy(&cstr[rep_len], with_cstr);
		rep_len += with_len;
		for( size_t i=0; i < amount; i++ ) {
			size_t const saved_offset = offset;
			size_t const relative_offs = csubstr_diff(&cstr[saved_offset], replace_cstr, nullptr);
			if( relative_offs==SIZE_MAX ) {
				break;
			}
			offset += relative_offs;
			size_t const span = offset - saved_offset;
			memmove(&cstr[rep_len], &cstr[saved_offset], span);
			rep_len += span;
			
			strcpy(&cstr[rep_len], with_cstr);
			rep_len += with_len;
			offset += replace_len;
		}
		memmove(&cstr[rep_len], &cstr[offset]);
		return cstr;
	} else {
		char *restrict new_cstr = calloc(len_calc + 1, sizeof *new_cstr);
		if( new_cstr==nullptr ) {
			return nullptr;
		}
		/// first copy contents up to the first offset.
		strncpy(&new_cstr[0], &cstr[0], offset);
		offset += replace_len;
		
		strcpy(&new_cstr[rep_len], with_cstr);
		rep_len += with_len;
		for( size_t i=0; i < amount; i++ ) {
			size_t const saved_offset = offset;
			size_t const relative_offs = csubstr_diff(&cstr[saved_offset], replace_cstr, nullptr);
			if( relative_offs==SIZE_MAX ) {
				break;
			}
			
			offset += relative_offs;
			size_t const span = offset - saved_offset;
			strncpy(&new_cstr[rep_len], &cstr[saved_offset], span);
			rep_len += span;
			
			strcpy(&new_cstr[rep_len], with_cstr);
			rep_len += with_len;
			offset += replace_len;
		}
		strcpy(&new_cstr[rep_len], &cstr[offset]);
		return new_cstr;
	}
}

HELPER_FUNC size_t harbol_idx_2D(size_t const x, size_t const y, size_t const y_len) {
	return y + (x * y_len);
}
HELPER_FUNC size_t harbol_idx_3D(size_t const x, size_t const y, size_t const z, size_t const y_len, size_t const z_len) {
	return z + (z_len*(y + (y_len * x)));
}

/** Array Calculation Formula:
 *  d    d
 *  Σ (  Π (lens[j]) * idxs[i] )
 * i=0 j=i+1
 * 
 * In better explanation:
 * run from the very LAST dimension index.
 * then add to it with the mult of the last dimension's length and previous dimension's index.
 * 
 * example1:: 2D array: y*1 + (y_len * x)
 * example2:: 3D array: z*1 + (z_len * (y + (y_len * x)))
 * example3:: 4D array: w*1 + (w_len * (z + (z_len * (y + (y_len * x)))))
 * 
 * Algebraically using 3D and 4D array:
 *     let all capital letters be the length of the respective dimension.
 *     z + (Z * (y + (Y * x)))
 *   = z + (Z * (y + Yx)) = z + (Zy + ZYx) = z + Zy + ZYx
 * 
 *     w + (W * (z + (Z * (y + (Y * x)))))
 *   = w + (W * (z + (Zy + ZYx)))
 *   = w + Wz + WZy + WZYx
 * 
 * Overall:
 * L_n*L_n-1*...*L_1*I_0 + L_n*L_n-1*...*L_2*I_1 + L_n*L_n-1*...*L_3*I_2 + L_n*L_n-1*...*L_4*I_3
 * 
 * So overall, the first index aka the last given, is multiplied by a product of all the lens minus 1.
 */
HELPER_FUNC size_t harbol_idx_ND_buf(size_t const n_dims, size_t const ARRAY_PARAM_FULL(idxs, n_dims), size_t const ARRAY_PARAM_FULL(lens, n_dims)) {
	size_t calcd_idx = 0;
	size_t stride = 1;
	for( size_t i = n_dims-1; i < n_dims; i-- ) {
		calcd_idx += idxs[i] * stride;
		stride *= lens[i];
	}
	return calcd_idx;
}
#endif /** HARBOL_BUFOPS_INCLUDED */