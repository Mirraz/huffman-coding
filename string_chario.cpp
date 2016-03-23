#include <assert.h>
#include "string_chario.h"

StringCharOut::StringCharOut(char *b_str, size_t b_size) : str(b_str), size(b_size), idx(0) {}

void StringCharOut::put(char c) {
	assert(idx < size);
	str[idx++] = c;
}

void StringCharOut::finish() {
}

size_t StringCharOut::get_length() const {
	return idx;
}

StringCharIn::StringCharIn(const char *b_str, size_t b_size) : str(b_str), size(b_size), idx(0) {}

int StringCharIn::get_with_eof() {
	if (idx == size) return ICharIn::EOF_VALUE;
	return (unsigned char)str[idx++];
}

