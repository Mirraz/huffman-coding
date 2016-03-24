#include <assert.h>
#include "string_chario.h"

StringCharOut::StringCharOut(char *b_str, size_t b_size) : array_symbol_out_type(b_str, b_size) {}

void StringCharOut::put(char c) {
	array_symbol_out_type::put(c);
}

void StringCharOut::finish() {
	array_symbol_out_type::finish();
}

size_t StringCharOut::get_length() const {
	return array_symbol_out_type::get_length();
}

StringCharIn::StringCharIn(const char *b_str, size_t b_size) : array_symbol_in_type(b_str, b_size) {}

bool StringCharIn::get(char &c) {
	return array_symbol_in_type::get(c);
}

int StringCharIn::get_with_eof() {
	char c;
	if (!get(c)) return ICharIn::EOF_VALUE;
	return c;
}

